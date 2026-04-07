#include "VoidAura.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Logger/Logger.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"
#include "KillAura.h"

#include <cmath>
#include <cstdlib>
#include <vector>

static VoidAuraFeature* self{ nullptr };

VoidAuraFeature::VoidAuraFeature()
    : Feature({ "card_voidaura", XS("VoidAura"), XS("Automatically strike nearby enemies"), XS("NONE"), FeatureTab::Combat })
{
    self = this;
}

HOOK_INSTALL(ConvertSyncTaskToMotionInfo, VoidAura_SyncTask)
{
    HOOK_RET_ORIG(ConvertSyncTaskToMotionInfo);
    void* __this = HOOK_ARG(ConvertSyncTaskToMotionInfo, 0);

    if (!self || !self->Active() || !__this) return ret_orig();

    auto* moveSyncTask = reinterpret_cast<MoveSyncTask*>(reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::BaseMoveSyncPlugin::_syncTask);
    if (!moveSyncTask) return ret_orig();

    const auto owner = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::BaseComponentPlugin::_owner);
    if (!owner) return ret_orig();

    const auto baseEntity = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(owner) + offsets::MoleMole::BaseComponent::_entity);
    if (!baseEntity)return ret_orig();

    const auto typeEntity = *reinterpret_cast<EntityType*>(reinterpret_cast<uintptr_t>(baseEntity) + offsets::MoleMole::BaseEntity::entityType);

    if (typeEntity == EntityType::Avatar)
    {
        const auto playerEntity = OtherUtils::AvatarManager();
        if (!playerEntity) return ret_orig();

        const auto playerID = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(playerEntity) + offsets::MoleMole::BaseEntity::runtimeID);
        const auto entityID = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(baseEntity) + offsets::MoleMole::BaseEntity::runtimeID);

        if (playerID == entityID)
        {
            currentPlayerPos = moveSyncTask->position;
            return ret_orig();
        }
    }

    if (currentPlayerPos.zero())
        return ret_orig();

    if (currentPlayerPos.distance(moveSyncTask->position) <= self->GetRadius())
        moveSyncTask->position.x += 1'000'000.f + static_cast<float>(std::rand() % 100'001);

    return ret_orig();
}

float VoidAuraFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    static const char* modes[] = { XS("Sync"), XS("RAPE") };

    y += GuiDropdownRow("voidaura_mode", XS("Mode"), modes,
        IM_ARRAYSIZE(modes), modeIndex_, ImVec2(x, y), width);

    y += GuiSliderRow("voidaura_radius", XS("Radius"),
        &radius_, 1.f, 50.f, XS("%.0f m"), ImVec2(x, y), width) + 10.f;

    y += GuiToggleTextRow("voidaura_draw", XS("Show radius"),
        XS("Enabled"), drawRadius_, ImVec2(x, y), width) + 6.f;

    y += GuiToggleTextRow("voidaura_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void VoidAuraFeature::OnDraw()
{
    if (!self->Active() || !drawRadius_)
        return;

    auto* camera = UnityUtils::GetMainCamera();
    if (!camera) return;

    const auto localAvatar = OtherUtils::AvatarManager();
    if (!localAvatar) return;

    // нельзя брать из ConvertSyncTaskToMotionInfo
    const auto centerWorld = UnityUtils::GetRelativePosition_Entity(localAvatar); 
    if (centerWorld.zero()) return;

    const auto centerScreen = OtherUtils::WorldToScreenPointCorrected(camera, centerWorld);
    if (centerScreen.z <= 1.f) return;

    const int screenH = UnityUtils::get_height();
    const ImVec2 center = { centerScreen.x, static_cast<float>(screenH) - centerScreen.y };

    constexpr float kRotSpeed = 0.02f;
    constexpr float kPulseSpeed = 0.15f;

    static float rotationTime = 0.f;
    static float pulseTime = 0.f;
    rotationTime += kRotSpeed;
    pulseTime += kPulseSpeed;

    const float alpha = 0.5f + (std::sin(pulseTime * 3.f) + 1.f) * 0.25f;
    const float rotAngle = rotationTime * 20.f * (3.14159265f / 180.f);

    constexpr int kSegments = 64;
    constexpr float kDegToRad = 3.14159265f / 180.f;

    static std::vector<ImVec2> points;
    static std::vector<float>  angles;
    points.clear();
    angles.clear();
    points.reserve(kSegments + 1);
    angles.reserve(kSegments + 1);

    for (int i = 0; i <= kSegments; ++i)
    {
        const float angle = (i * 360.f / kSegments) * kDegToRad;
        const Vector3 worldPt = {
            centerWorld.x + radius_ * std::cos(angle),
            centerWorld.y,
            centerWorld.z + radius_ * std::sin(angle)
        };

        const auto screenPt = OtherUtils::WorldToScreenPointCorrected(camera, worldPt);
        if (screenPt.z > 1.f)
        {
            points.push_back({ screenPt.x, static_cast<float>(screenH) - screenPt.y });
            angles.push_back(angle);
        }
    }

    if (points.size() < 3)
        return;

    auto* drawList = ImGui::GetForegroundDrawList();
    if (!drawList)
        return;

    const ImU32 tintColor = IM_COL32(255, 255, 255, static_cast<int>(255.f * alpha));

    for (size_t i = 0; i + 1 < points.size(); ++i)
    {
        const float a1 = angles[i] - rotAngle;
        const float a2 = angles[i + 1] - rotAngle;

        constexpr ImVec2 uvCenter = { 0.5f, 0.5f };
        const ImVec2 uv1 = { 0.5f + 0.5f * std::cos(a1), 0.5f + 0.5f * std::sin(a1) };
        const ImVec2 uv2 = { 0.5f + 0.5f * std::cos(a2), 0.5f + 0.5f * std::sin(a2) };

        drawList->AddImageQuad(
            g_radiusSrv,
            center, points[i], points[i + 1], center,
            uvCenter, uv1, uv2, uvCenter,
            tintColor
        );
    }
}

void VoidAuraFeature::OnEnable()
{
    if (auto* ka = FindFeature<KillAuraFeature>())
        ka->SetEnabled(false);
}

void VoidAuraFeature::OnShutdown()
{
    SetEnabled(false);
}