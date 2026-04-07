#include "AirJump.h"
#include "../../GUI/gui.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

static AirJumpFeature* self{ nullptr };
AirJumpFeature::AirJumpFeature()
    : Feature({ "card_air_jump", XS("Jump Setting"), XS("jump jump jump"), XS("NONE"), FeatureTab::Move })
{
    self = this;
}

namespace {

    std::atomic<bool> g_jumpHold{ false };

    HOOK_INSTALL(HumanoidMoveFSM_LateTick, AirJump_LateTick)
    {
        HOOK_RET_ORIG(HumanoidMoveFSM_LateTick);
        void* __this = HOOK_ARG(HumanoidMoveFSM_LateTick, 0);

        if (!self || !self->Active() || !__this)return ret_orig();

        auto* moveDataPtr = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::HumanoidMoveFSM::_moveData);
        if (!moveDataPtr) return ret_orig();

        auto* moveData = reinterpret_cast<VCHumanoidMoveData*>(moveDataPtr);
        moveData->jumpXZVelocityScale = self->GetExtraJumps();

        if (self->GetNoJumpDelay())moveData->tryDoJump = g_jumpHold ? 1 : 0;

        return ret_orig();
    }

}

void (*OnPointerUp_Orig)(void*, void*) = nullptr;
void OnPointerUp_Hook(void* monoJumpButton, void* eventData)
{
    if (!self || !self->Active() || !monoJumpButton)
        return OnPointerUp_Orig(monoJumpButton, eventData);

    g_jumpHold = false;
    OnPointerUp_Orig(monoJumpButton, eventData);
}

void (*OnRealPointerDown_Orig)(void*, void*) = nullptr;
void OnRealPointerDown_Hook(void* monoJumpButton, void* eventData)
{
    if (!self || !self->Active() || !monoJumpButton)
        return OnRealPointerDown_Orig(monoJumpButton, eventData);

    OnRealPointerDown_Orig(monoJumpButton, eventData);
    g_jumpHold = true;
}

//void* (*MovePositionAndRotationByCenter_Orig)(void*, Vector3, Vector3, float) = nullptr;
//void* MovePositionAndRotationByCenter_Hook(void* __this, Vector3 targetPos, Vector3 targetDir, float dt)
//{
//    float boost = 2.0f;
//    float maxStep = 0.15f - 0.35f;
//
//    float len = sqrtf(targetDir.x * targetDir.x + targetDir.y * targetDir.y + targetDir.z * targetDir.z);
//    if (len > 0.0001f) {
//        float nx = targetDir.x / len;
//        float ny = targetDir.y / len;
//        float nz = targetDir.z / len;
//
//
//        if (ny < 0) {
//            ny = -ny; 
//        }
//
//        float step = boost * dt;
//        if (step > maxStep) step = maxStep;
//
//        //targetPos.x += nx * step;
//        targetPos.y += ny;//*step;
//       // targetPos.z += nz * step;
//    }
//
//    return MovePositionAndRotationByCenter_Orig(__this, targetPos, targetDir, dt);
//}

float AirJumpFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiSliderRow("jump_velo", XS("Long Jump"),
        &extraJumps_, 1.f, 25.f, "%.0f", ImVec2(x, y), width, true) + 10.f;

    y += GuiToggleTextRow("jump_delay", XS("No Jump Delay"),
        XS("Enabled"), noJumpDelay_, ImVec2(x, y), width) + 6.f;

    y += GuiToggleTextRow("feel_free", XS("Bhop???"),
        XS("Enabled"), bhopStyle_, ImVec2(x, y), width) + 6.f;

    y += GuiToggleTextRow("air_jump_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void AirJumpFeature::OnShutdown()
{
    SetEnabled(false);
}
