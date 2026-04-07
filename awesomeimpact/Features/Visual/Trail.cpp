#include "Trail.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"
#include "../Move/BowTp.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>

static TrailFeature* self{ nullptr };
TrailFeature::TrailFeature()
    : Feature({ "trail", XS("Trail"), XS(":p"), "NONE", FeatureTab::Visual })
{
    self = this;
}

namespace {

    enum TrailMode { TrailMode_Line = 0, TrailMode_Star, TrailMode_Fire, TrailMode_Idk, TrailMode_Count };

    ABKFGEMPBNN g_effectHandle{};
    void* g_effectOwner = nullptr;
    bool        g_effectSpawned = false;
    char        g_effectName[260]{};
    int         g_lastMode = -1;

    inline void ResetEffectState()
    {
        g_effectHandle = {};
        g_effectOwner = nullptr;
        g_effectSpawned = false;
        g_effectName[0] = '\0';
    }

    inline const char* EffectNameForMode(int mode)
    {
        switch (mode) {
        case TrailMode_Star: return XS("Eff_NPC_Kanban_Paimon_Standby");
        case TrailMode_Fire: return XS("Eff_Ambor_Arrow_Fire_Trail");
        case TrailMode_Idk:  return XS("Eff_Arrow_Fly");
        default: return nullptr;
        }
    }

    inline Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t)
    {
        const float t2 = t * t, t3 = t2 * t;
        return {
            0.5f * (2 * p1.x + (-p0.x + p2.x) * t + (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t2 + (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t3),
            0.5f * (2 * p1.y + (-p0.y + p2.y) * t + (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t2 + (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t3),
            0.5f * (2 * p1.z + (-p0.z + p2.z) * t + (2 * p0.z - 5 * p1.z + 4 * p2.z - p3.z) * t2 + (-p0.z + 3 * p1.z - 3 * p2.z + p3.z) * t3),
        };
    }

    inline float SmoothFade(float t)
    {
        t = std::clamp(t, 0.f, 1.f);
        return t * t * (3.f - 2.f * t);
    }

    inline int ToByte(float v)
    {
        return static_cast<int>(std::clamp(v, 0.f, 1.f) * 255.f + 0.5f);
    }


    HOOK_INSTALL(HumanoidMoveFSM_LateTick, Trail_LateTick)
    {
        HOOK_RET_ORIG(HumanoidMoveFSM_LateTick);
        void* __this = HOOK_ARG(HumanoidMoveFSM_LateTick, 0);

        if (!self || !self->Active() || !__this)
            return ret_orig();

        const int mode = self->TrailModeIndex();
        if (mode != g_lastMode) { ResetEffectState(); g_lastMode = mode; }
        if (mode == TrailMode_Line)
            return ret_orig();

        const char* fxName = EffectNameForMode(mode);
        if (!fxName || !*fxName)
            return ret_orig();

        const auto owner = *reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::BaseComponentPlugin::_owner);
        if (!owner)
            return ret_orig();

        const auto baseEntity = *reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(owner) + offsets::MoleMole::BaseComponent::_entity);
        if (!baseEntity || OtherUtils::GetEntityType(baseEntity) != EntityType::Avatar)
            return ret_orig();

        const auto localAvatar = OtherUtils::AvatarManager();
        if (!localAvatar || baseEntity != localAvatar)
            return ret_orig();

        const bool nameChanged = std::strcmp(g_effectName, fxName) != 0;
        const bool ownerChanged = g_effectOwner != baseEntity;

        if (!nameChanged && !ownerChanged && g_effectSpawned)
            return ret_orig();

        ResetEffectState();

        auto* mgr = OtherUtils::EffectManager();
        if (!mgr) return ret_orig();

        auto* fxStr = UnityUtils::PtrToStringAnsi((void*)fxName);
        if (!fxStr) return ret_orig();

        Quaternion rot{}; rot.w = 1.f;
        const float scale = std::clamp(self->Thickness(), 0.1f, 5.f);

        g_effectHandle = EffectUtils::CreateUnindexedEntityEffect(
            mgr, baseEntity, fxStr,
            NullNullable<Vector3>(),
            MakeNullable(rot),
            MakeNullable(Vector3{ scale, scale, scale }),
            NullNullable<Vector3>(),
            baseEntity, nullptr,
            NullNullable<Vector3>(),
            false, false, fxStr, false,
            NullNullable<OLOHHCPGJLF>()
        );

        g_effectOwner = baseEntity;
        g_effectSpawned = true;
        strncpy_s(g_effectName, sizeof(g_effectName), fxName, _TRUNCATE);

        return ret_orig();
    }

}

float TrailFeature::OnGUI(const ImVec2& detailStart, float width)
{
    static const char* modes[] = { XS("Line"), XS("Star"), XS("Fire trail"), XS("Idk trail") };

    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiDropdownRow("trail_mode", XS("Mode"), modes,
        IM_ARRAYSIZE(modes), trailMode_, ImVec2(x, y), width) + 8.f;

    y += GuiColorButtonRow("trail_color", XS("Color"), color_, ImVec2(x, y), width) + 8.f;

    y += GuiSliderRow("trail_thickness", XS("Thickness"),
        &thickness_, 0.5f, 3.f, XS("%.1f px"), ImVec2(x, y), width, true);
    y += GuiSliderRow("trail_duration", XS("Length"),
        &durationSec_, 0.5f, 10.f, XS("%.1f s"), ImVec2(x, y), width, true) + 10.f;

    y += GuiToggleTextRow("arrow_trail_enabled", XS("Arrow Trail"),
        XS("Enabled"), arrowTrailEnabled_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void TrailFeature::OnDraw()
{
    if (!self || !self->Active()) return;

    static int lastLineMode = -1;
    const int mode = trailMode_;
    if (mode != lastLineMode) { points_.clear(); elapsedTime_ = 0.0; lastLineMode = mode; }
    if (mode != TrailMode_Line) return;

    auto* playerEntity = OtherUtils::AvatarManager();
    if (!playerEntity) return;

    BowTpFeature::SetArrowTrackEnabled(arrowTrailEnabled_);
    void* trackEntity = playerEntity;
    if (arrowTrailEnabled_)
    {
        auto* arrow = BowTpFeature::GetArrowEntity();
        if (!arrow) { points_.clear(); return; }
        trackEntity = arrow;
    }

    const auto playerPos = UnityUtils::GetRelativePosition_Entity(trackEntity);
    if (playerPos.zero()) return;

    auto* camera = UnityUtils::GetMainCamera();
    if (!camera) return;

    auto* dl = ImGui::GetForegroundDrawList();
    if (!dl) return;

    float dt = ImGui::GetIO().DeltaTime;
    if (dt <= 0.f) dt = UnityUtils::Time_get_deltaTime();
    if (dt <= 0.f || dt > 0.5f) return;

    elapsedTime_ += static_cast<double>(dt);
    const double now = elapsedTime_;

    if (points_.empty() || playerPos.distance(points_.back().world) >= minSegmentDist_)
    {
        points_.push_back({ playerPos, now });
        while (points_.size() > 2048) points_.pop_front();
    }

    const double cutoff = now - static_cast<double>(durationSec_);
    while (!points_.empty() && points_.front().timeStamp < cutoff) points_.pop_front();
    if (points_.size() < 2) return;

    const int screenH = UnityUtils::get_height();
    const int colR = ToByte(color_.x), colG = ToByte(color_.y), colB = ToByte(color_.z);
    const float baseAlpha = color_.w;
    const int ptCount = static_cast<int>(points_.size());
    const int subs = (std::max)(subdivisions_, 1);

    struct ScreenPt { ImVec2 pos; float age; bool valid; };
    std::vector<ScreenPt> curve;
    curve.reserve((ptCount - 1) * subs + 1);

    const double oldest = points_.front().timeStamp;
    const double newest = points_.back().timeStamp;
    const double span = newest - oldest;
    if (span < 0.001) return;

    for (int i = 0; i < ptCount - 1; ++i)
    {
        const int i0 = (std::max)(i - 1, 0);
        const int i2 = i + 1;
        const int i3 = (std::min)(i + 2, ptCount - 1);

        for (int s = 0; s < subs; ++s)
        {
            const float frac = static_cast<float>(s) / static_cast<float>(subs);
            const auto wp = CatmullRom(points_[i0].world, points_[i].world, points_[i2].world, points_[i3].world, frac);
            const double ptTime = points_[i].timeStamp + (points_[i2].timeStamp - points_[i].timeStamp) * frac;
            const float age = 1.f - static_cast<float>((ptTime - oldest) / span);

            const auto sc = OtherUtils::WorldToScreenPointCorrected(camera, wp);
            curve.push_back({ { sc.x, static_cast<float>(screenH) - sc.y }, age, sc.z > 1.f });
        }
    }
    {
        const auto sc = OtherUtils::WorldToScreenPointCorrected(camera, points_.back().world);
        curve.push_back({ { sc.x, static_cast<float>(screenH) - sc.y }, 0.f, sc.z > 1.f });
    }

    if (curve.size() < 2) return;

    const float baseThick = (std::max)(thickness_, 0.5f);
    const int coreR = (std::min)(colR + (255 - colR) / 3, 255);
    const int coreG = (std::min)(colG + (255 - colG) / 3, 255);
    const int coreB = (std::min)(colB + (255 - colB) / 3, 255);

    size_t idx = 0;
    while (idx < curve.size())
    {
        while (idx < curve.size() && !curve[idx].valid) ++idx;
        if (idx >= curve.size()) break;

        const size_t runStart = idx;
        while (idx < curve.size() && curve[idx].valid) ++idx;

        if (static_cast<int>(idx - runStart) < 2) continue;

        for (size_t j = runStart; j + 1 < idx; ++j)
        {
            const auto& a = curve[j];
            const auto& b = curve[j + 1];

            const float avgAge = (a.age + b.age) * 0.5f;
            const float alpha = (1.f - SmoothFade(avgAge)) * baseAlpha;
            if (alpha < 0.005f) continue;

            const float segThick = (std::max)(baseThick * (1.f - avgAge * 0.7f), 0.3f);
            const int glowA = static_cast<int>(alpha * 45.f);
            const int baseA = static_cast<int>(alpha * 200.f);
            const int innerA = static_cast<int>(alpha * 255.f);

            if (glowA < 1 && baseA < 1) continue;

            if (glowA > 0)  dl->AddLine(a.pos, b.pos, IM_COL32(colR, colG, colB, glowA), segThick * 3.f);
            if (baseA > 0)  dl->AddLine(a.pos, b.pos, IM_COL32(colR, colG, colB, baseA), segThick);
            if (innerA > 0) dl->AddLine(a.pos, b.pos, IM_COL32(coreR, coreG, coreB, innerA / 2), segThick * 0.45f);
        }
    }
}

void TrailFeature::OnShutdown()
{
    SetEnabled(false);
    points_.clear();
    elapsedTime_ = 0.0;
    ResetEffectState();
    g_lastMode = -1;
}