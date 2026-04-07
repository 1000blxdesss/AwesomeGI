#include "Spin.h"
#include "../../GUI/gui.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#include <cmath>

static SpinFeature* self{ nullptr };
SpinFeature::SpinFeature()
    : Feature({ "card_spin", XS("Spin"), XS("KurukuruKururing"), XS("NONE"), FeatureTab::Move })
{
    self = this;
}

namespace {

    float g_spinAngle = 0.f;

    constexpr float kDegToRad = 0.01745329f;
    constexpr float kHalfDegToRad = 0.00872664626f;

    inline Quaternion EulerY(float yawDeg)
    {
        const float half = yawDeg * kHalfDegToRad;
        return { 0.f, std::sin(half), 0.f, std::cos(half) };
    }

    HOOK_INSTALL(HumanoidMoveFSM_LateTick, Spin_LateTick)
    {
        HOOK_RET_ORIG(HumanoidMoveFSM_LateTick);
        void* __this = HOOK_ARG(HumanoidMoveFSM_LateTick, 0);

        if (!self || !self->Active() || !__this)
            return ret_orig();

        auto* rb = *reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::HumanoidMoveFSM::_rigidbody);
        if (!rb)
            return ret_orig();

        auto* transform = UnityUtils::Component_get_transform(rb);
        if (!transform) return ret_orig();

        g_spinAngle += self->GetSpeed() * UnityUtils::Time_get_deltaTime() * 36.f;
        if (g_spinAngle >= 360.f) g_spinAngle -= 360.f;

        if (self->GetModeIndex() == 0)
            UnityUtils::Transform_set_eulerAngles(transform, { 0.f, g_spinAngle, 0.f });

        return ret_orig();
    }
    // для глобала достаточно поставить вызов после ориг метода, либо синхронизировать ручечками
   /* HOOK_INSTALL(ConvertSyncTaskToMotionInfo, Spin_SyncTask)
    {
        HOOK_RET_ORIG(ConvertSyncTaskToMotionInfo);
        void* __this = HOOK_ARG(ConvertSyncTaskToMotionInfo, 0);

        ret_orig();

        if (!self || !self->Active() || !__this)
            return;

        if (self->GetModeIndex() == 1)
        {
            auto* syncTask = reinterpret_cast<MoveSyncTask*>(reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::BaseMoveSyncPlugin::_syncTask);

            syncTask->rotation = EulerY(g_spinAngle);

            const float rad = g_spinAngle * kDegToRad;
            syncTask->forward = { std::sin(rad), 0.f, std::cos(rad) };
        }
    }*/

}

float SpinFeature::OnGUI(const ImVec2& detailStart, float width)
{
    static const char* modes[] = { XS("Local"), XS("Global") };

    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiDropdownRow("spin_mode", XS("Mode"), modes,
        IM_ARRAYSIZE(modes), modeIndex_, ImVec2(x, y), width) + 6.f;

    y += GuiSliderRow("spin_speed", XS("Speed"),
        &speed_, 0.f, 360.f, "%.0f", ImVec2(x, y), width, true) + 10.f;

    y += GuiToggleTextRow("spin_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void SpinFeature::OnShutdown()
{
    hold_ = false;
}