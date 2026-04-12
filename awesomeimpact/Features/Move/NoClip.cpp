#include "NoClip.h"
#include "../../Utils/HookRegistry.h"
#include "../../GUI/gui.h"
#include "../../Utils/XorStr.h"

#include <optional>

static NoClipFeature* self{ nullptr };
NoClipFeature::NoClipFeature()
    : Feature({ "card_noclip", XS("NoClip"), XS("Move freely through terrain"), XS("NONE"), FeatureTab::Move })
{
    self = this;
}

namespace
{
    std::optional<bool> g_originalKinematic;

    void SetCollidersEnabled(void* entity, bool enabled)
    {
        if (!entity)return;

        auto* colliders = *reinterpret_cast<GenericList**>(reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::_colliders);
        if (!colliders || !colliders->items || colliders->size <= 0)return;

        auto** items = reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(colliders->items) + 0x20);

        for (int i = 0; i < colliders->size; ++i)
        {
            if (items[i])UnityUtils::Collider_set_enabled(items[i], enabled);
        }
    }

    Vector3 BuildMoveDirection(void* cameraTransform)
    {
        if (!cameraTransform) return {};

        auto forward = UnityUtils::Transform_get_forward(cameraTransform);
        auto right = UnityUtils::Transform_get_right(cameraTransform);
        forward.y = 0.f;
        right.y = 0.f;

        Vector3 dir{};
        if (GetAsyncKeyState('W') & 0x8000) dir = dir + forward;
        if (GetAsyncKeyState('S') & 0x8000) dir = dir - forward;
        if (GetAsyncKeyState('D') & 0x8000) dir = dir + right;
        if (GetAsyncKeyState('A') & 0x8000) dir = dir - right;

        const float horiz = std::sqrt(dir.x * dir.x + dir.z * dir.z);
        if (horiz > 0.001f)
        {
            dir.x /= horiz;
            dir.z /= horiz;
        }

        if (GetAsyncKeyState(VK_SPACE) & 0x8000) dir.y += 1.0f;
        if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) dir.y -= 1.0f;

        return dir;
    }

    void RestoreState(void* localAvatar, void* rigidbody)
    {
        if (!g_originalKinematic)return;
        if (localAvatar)SetCollidersEnabled(localAvatar, true);
        if (rigidbody)UnityUtils::Rigidbody_set_isKinematic(rigidbody, *g_originalKinematic);

        g_originalKinematic.reset();
    }
}

HOOK_INSTALL(HumanoidMoveFSM_LateTick, NoClip_LateTick)
{
    HOOK_RET_ORIG(HumanoidMoveFSM_LateTick);
    auto* __this = HOOK_ARG(HumanoidMoveFSM_LateTick, 0);
    if (!__this)return ret_orig();

    auto* localAvatar = OtherUtils::AvatarManager();
    if (!localAvatar)
    {
        g_originalKinematic.reset();
        return ret_orig();
    }

    const auto owner = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::BaseComponentPlugin::_owner);
    if (!owner)return ret_orig();

    const auto baseEntity = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(owner) + offsets::MoleMole::BaseComponent::_entity);
    if (!baseEntity || baseEntity != localAvatar)return ret_orig();

    auto* rigidbody = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::HumanoidMoveFSM::_rigidbody);
    if (!rigidbody)return ret_orig();

    const bool active = self && self->Active();
    if (!active)
    {
        RestoreState(localAvatar, rigidbody);
        return ret_orig();
    }

    if (!g_originalKinematic)g_originalKinematic = UnityUtils::Rigidbody_get_isKinematic(rigidbody);
    SetCollidersEnabled(localAvatar, false);

    auto* camera = UnityUtils::GetMainCamera();
    auto* cameraTransform = camera ? UnityUtils::Component_get_transform(camera) : nullptr;
    const auto dir = BuildMoveDirection(cameraTransform);
    const float deltaTime = UnityUtils::Time_get_deltaTime();

    if (self->GetModeIndex() == 1)
    {
        UnityUtils::Rigidbody_set_isKinematic(rigidbody, true);
        UnityUtils::Rigidbody_set_velocity(rigidbody, {});

        if (dir.zero())return;

        auto* rbTransform = UnityUtils::Component_get_transform(rigidbody);
        if (!rbTransform)return;

        const auto currentPos = UnityUtils::Transform_get_position(rbTransform);
        if (currentPos.zero())return;

        UnityUtils::Rigidbody_MovePosition(rigidbody,currentPos + dir * self->GetSpeed() * deltaTime);
        return;
    }

    UnityUtils::Rigidbody_set_isKinematic(rigidbody, false);
    ret_orig();
    UnityUtils::Rigidbody_set_velocity(rigidbody, {});

    if (dir.zero())return;

    const auto currentPos = UnityUtils::GetRelativePosition_Entity(localAvatar);
    if (currentPos.zero())return;

    UnityUtils::SetRelativePosition(localAvatar,currentPos + dir * self->GetSpeed() * deltaTime,true);
}

float NoClipFeature::OnGUI(const ImVec2& detailStart, float width)
{
    static const char* modes[] = { XS("Legit"), XS("Desync") };

    float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    float row = GuiDropdownRow("noclip_mode", XS("Mode"), modes, IM_ARRAYSIZE(modes), modeIndex_, ImVec2(x, y), width);
    y += row + 6.f;

    y += GuiSliderRow("noclip_speed", XS("Speed"), &speed_, 1.f, 50.f, "%.1f", ImVec2(x, y), width, true);
    y += 10.f;

    float toggleHeight = GuiToggleTextRow("noclip_hold", XS("Key"), XS("Hold"), hold_, ImVec2(x, y), width);
    y += toggleHeight + 6.f;
    return (y - detailStart.y) + 12.f;
}

void NoClipFeature::OnShutdown()
{
    SetEnabled(false);
}
