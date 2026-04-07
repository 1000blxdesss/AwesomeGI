#include "BetteFly.h"
#include "../../GUI/gui.h"
#include "../Visual/ProfileChanger.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#include <cmath>
#include <optional>

static BetterFlyFeature* self{ nullptr };
BetterFlyFeature::BetterFlyFeature()
    : Feature({ "card_better_fly", XS("Better Fly"), XS("fly like a babochka :))"), XS("NONE"), FeatureTab::Move })
{
    self = this;
}

namespace {

    std::optional<Vector3> g_savedPreFlyVelocity;

    inline void* GetMoveData(void* fsm)
    {
        return *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(fsm) + offsets::MoleMole::HumanoidMoveFSM::_moveData);
    }

    inline void* GetRigidbody(void* fsm)
    {
        return *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(fsm) + offsets::MoleMole::HumanoidMoveFSM::_rigidbody);
    }

    inline Vector3 BuildMoveDirection(void* camera, float speed)
    {
        auto* camTr = camera ? UnityUtils::Component_get_transform(camera) : nullptr;
        if (!camTr)
            return {};

        auto fwd = UnityUtils::Transform_get_forward(camTr);
        auto right = UnityUtils::Transform_get_right(camTr);
        fwd.y = 0.f;
        right.y = 0.f;

        Vector3 dir{};
        if (GetAsyncKeyState('W') & 0x8000) dir = dir + fwd;
        if (GetAsyncKeyState('S') & 0x8000) dir = dir - fwd;
        if (GetAsyncKeyState('D') & 0x8000) dir = dir + right;
        if (GetAsyncKeyState('A') & 0x8000) dir = dir - right;

        const float len = std::sqrt(dir.x * dir.x + dir.z * dir.z);
        if (len > 0.0001f)
            return { (dir.x / len) * speed, 0.f, (dir.z / len) * speed };

        return {};
    }

    HOOK_INSTALL(HumanoidMoveFSM_LateTick, BetterFly_LateTick)
    {
        HOOK_RET_ORIG(HumanoidMoveFSM_LateTick);
        void* __this = HOOK_ARG(HumanoidMoveFSM_LateTick, 0);

        if (!__this)
            return ret_orig();

        const bool active = self && self->Active();

        const auto owner = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::BaseComponentPlugin::_owner);
        if (!owner)
            return ret_orig();

        const auto baseEntity = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(owner) + offsets::MoleMole::BaseComponent::_entity);
        const auto localAvatar = OtherUtils::AvatarManager();
        if (!localAvatar)
        {
            g_savedPreFlyVelocity.reset();
            return ret_orig();
        }

        if (!baseEntity || baseEntity != localAvatar) return ret_orig();

        auto* moveDataPtr = GetMoveData(__this);
        if (!moveDataPtr) return ret_orig();
        auto* moveData = reinterpret_cast<VCHumanoidMoveData*>(moveDataPtr);

        const bool blockFlightInput = IsProfileMenuVisible();
        const bool upPressed = !blockFlightInput && (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
        const bool downPressed = !blockFlightInput && (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        const bool movePressed = !blockFlightInput && (
            (GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState('A') & 0x8000) ||
            (GetAsyncKeyState('S') & 0x8000) || (GetAsyncKeyState('D') & 0x8000));

        auto* rb = GetRigidbody(__this);
        const Vector3 preTickVel = rb ? UnityUtils::Rigidbody_get_velocity(rb) : Vector3{};

        if (active && self->GetFlyCloakUnlock()) moveData->forceAirStateFly = 1;

        if (active)
        {
            moveData->tryFlyUp = upPressed ? 1 : 0;
            moveData->tryFlyDown = downPressed ? 1 : 0;
        }

        ret_orig();
        rb = GetRigidbody(__this);

        if (!active)
        {
            if (g_savedPreFlyVelocity && rb)UnityUtils::Rigidbody_set_velocity(rb, *g_savedPreFlyVelocity);
            g_savedPreFlyVelocity.reset(); 
            return;
        }

        moveDataPtr = GetMoveData(__this);
        moveData = reinterpret_cast<VCHumanoidMoveData*>(moveDataPtr);
        if (self->GetFlyCloakUnlock() && moveData)moveData->forceAirStateFly = 1;

        const auto curState = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::HumanoidMoveFSM::_curState);
        if (!curState) return;

        const int stateID = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(curState) + offsets::MoleMole::HumanoidMoveFSMBaseMoveState::k__BackingField);
        const bool isFlying = static_cast<FSMStateID>(stateID) == FSMStateID::Fly;

        if (!isFlying)
        {
            if (g_savedPreFlyVelocity && rb) UnityUtils::Rigidbody_set_velocity(rb, *g_savedPreFlyVelocity);
            g_savedPreFlyVelocity.reset();
            return;
        }

        if (!g_savedPreFlyVelocity) g_savedPreFlyVelocity = preTickVel;
        if (!rb) return;

        auto vel = UnityUtils::Rigidbody_get_velocity(rb);

        constexpr float kVertSpeed = 3.5f;
        vel.y = upPressed ? kVertSpeed : downPressed ? -kVertSpeed : 0.f;

        if (blockFlightInput)
        {
            vel.x = 0.f;
            vel.z = 0.f;
        }
        else if (movePressed)
        {
            const float horizSpeed = self->GetFlySpeed();
            const float h = std::sqrt(vel.x * vel.x + vel.z * vel.z);

            if (h > 0.0001f)
            {
                const float mul = horizSpeed / h;
                vel.x *= mul;
                vel.z *= mul;
            }
            else
            {
                const auto dir = BuildMoveDirection(UnityUtils::GetMainCamera(), horizSpeed);
                vel.x = dir.x;
                vel.z = dir.z;
            }
        }
        UnityUtils::Rigidbody_set_velocity(rb, vel);
    }
}

float BetterFlyFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiSliderRow("betterFly_speed", XS("Fly Speed"),
        &flySpeed_, 1.f, 50.f, "%.0f", ImVec2(x, y), width, true) + 10.f;

    y += GuiToggleTextRow("better_fly_flyCloak", XS("FlyCloak Unlock"),
        XS("Enabled"), flyCloakUnlock_, ImVec2(x, y), width) + 6.f;

    y += GuiToggleTextRow("better_fly_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void BetterFlyFeature::OnShutdown()
{
    g_savedPreFlyVelocity.reset();
    SetEnabled(false);
}
