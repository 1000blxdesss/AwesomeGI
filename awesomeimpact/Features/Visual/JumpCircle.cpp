#include "JumpCircle.h"
#include "../../GUI/gui.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"
#include "../../globals.h"

#include <cmath>
#include <vector>
#include <algorithm>

static JumpCircleFeature* self{ nullptr };
JumpCircleFeature::JumpCircleFeature()
    : Feature({ "card_jump_circle", XS("Jump Circle"), XS("Draw a circle after jump"), "NONE", FeatureTab::Visual })
{
    self = this;
}

namespace {

    int g_lastStateID = -1;

    struct JumpRing {
        Vector3 center;
        float time;
        float capY;
    };

    std::vector<JumpRing> g_rings;

    HOOK_INSTALL(HumanoidMoveFSM_LateTick, JumpCircle_LateTick)
    {
        HOOK_RET_ORIG(HumanoidMoveFSM_LateTick);
        void* __this = HOOK_ARG(HumanoidMoveFSM_LateTick, 0);

        if (!self || !self->Active() || !__this)
            return ret_orig();

        const auto curState = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::HumanoidMoveFSM::_curState);
        if (!curState)return ret_orig();

        const int stateID = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(curState) + offsets::MoleMole::HumanoidMoveFSMBaseMoveState::k__BackingField);

        constexpr int kJump = static_cast<int>(FSMStateID::Jump);

        if (stateID == kJump && g_lastStateID != kJump)
        {
            auto center = currentPlayerPos;
            float capY = center.y;

            if (const auto localAvatar = OtherUtils::AvatarManager())
            {
                const auto pos = UnityUtils::GetRelativePosition_Entity(localAvatar);
                if (!pos.zero())
                {
                    center = pos;
                    capY = pos.y;
                }
            }

            const float gy = UnityUtils::CalcCurrentGroundHeight(center.x, center.z);
            if (gy > 0.f) center.y = gy;
            if (capY < center.y) capY = center.y;

            g_rings.push_back({ center, 0.f, capY });
            if (g_rings.size() > 64)
                g_rings.erase(g_rings.begin());
        }

        g_lastStateID = stateID;
        return ret_orig();
    }

}

float JumpCircleFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiColorButtonRow("jump_circle_color", XS("Color"), color_, ImVec2(x, y), width) + 8.f;

    y += GuiSliderRow("jump_circle_radius", XS("Radius"),
        &maxRadius_, 1.f, 25.f, XS("%.1f m"), ImVec2(x, y), width, true);
    y += GuiSliderRow("jump_circle_speed", XS("Speed"),
        &speed_, 0.2f, 20.f, XS("%.1f m/s"), ImVec2(x, y), width, true) + 10.f;

    y += GuiToggleTextRow("jump_hold_key", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void JumpCircleFeature::OnDraw()
{
    if (!self || !self->Active() || g_rings.empty())
        return;

    auto* camera = UnityUtils::GetMainCamera();
    if (!camera) return;

    auto* dl = ImGui::GetForegroundDrawList();
    if (!dl) return;

    float dt = ImGui::GetIO().DeltaTime;
    if (dt <= 0.f) dt = UnityUtils::Time_get_deltaTime();

    if (speed_ <= 0.001f || maxRadius_ <= 0.001f) return;
    const float life = maxRadius_ / speed_;
    if (life <= 0.001f) return;

    const int screenH = UnityUtils::get_height();
    constexpr int kSegments = 64;
    constexpr float kDegToRad = 3.14159265f / 180.f;

    const int colR = static_cast<int>(std::clamp(color_.x, 0.f, 1.f) * 255.f + 0.5f);
    const int colG = static_cast<int>(std::clamp(color_.y, 0.f, 1.f) * 255.f + 0.5f);
    const int colB = static_cast<int>(std::clamp(color_.z, 0.f, 1.f) * 255.f + 0.5f);

    for (size_t i = 0; i < g_rings.size();)
    {
        auto& ring = g_rings[i];
        ring.time += dt;

        const float t = ring.time / life;
        if (t >= 1.f) { g_rings.erase(g_rings.begin() + i); continue; }

        const float radius = speed_ * ring.time;
        if (radius <= 0.01f) { ++i; continue; }

        const float alpha = std::clamp(1.f - t, 0.f, 1.f);
        const int baseA = static_cast<int>(alpha * 220.f);
        const int glowA = static_cast<int>(alpha * 90.f);
        if (baseA < 1) { ++i; continue; }

        std::vector<ImVec2> pts;
        pts.reserve(kSegments + 1);

        for (int s = 0; s <= kSegments; ++s)
        {
            const float angle = (s * 360.f / kSegments) * kDegToRad;
            Vector3 wp = {
                ring.center.x + radius * std::cos(angle),
                ring.center.y,
                ring.center.z + radius * std::sin(angle)
            };

            const float gy = UnityUtils::CalcCurrentGroundHeight(wp.x, wp.z);
            if (gy > 0.f) wp.y = gy;
            if (wp.y > ring.capY) wp.y = ring.capY;

            const auto screen = OtherUtils::WorldToScreenPointCorrected(camera, wp);
            if (screen.z > 1.f)
                pts.push_back({ screen.x, static_cast<float>(screenH) - screen.y });
        }

        if (pts.size() >= 2)
        {
            const float baseThick = (std::max)(thickness_, 0.6f);
            const float glowThick = baseThick * 2.2f;
            const float innerThick = baseThick * 0.6f;

            dl->AddPolyline(pts.data(), static_cast<int>(pts.size()), IM_COL32(colR, colG, colB, glowA), ImDrawFlags_None, glowThick);
            dl->AddPolyline(pts.data(), static_cast<int>(pts.size()), IM_COL32(colR, colG, colB, baseA), ImDrawFlags_None, baseThick);
            dl->AddPolyline(pts.data(), static_cast<int>(pts.size()), IM_COL32(colR, colG, colB, baseA / 2), ImDrawFlags_None, innerThick);
        }

        ++i;
    }
}

void JumpCircleFeature::OnShutdown()
{
    SetEnabled(false);
    g_rings.clear();
}