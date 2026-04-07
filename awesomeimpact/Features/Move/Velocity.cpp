#include "Velocity.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

static VelocityFeature* self{ nullptr };
VelocityFeature::VelocityFeature()
    : Feature({ "card_velocity", XS("Velocity"), XS("Speeeeeed better than Speed"), XS("NONE"), FeatureTab::Move })
{
    self = this;
}

namespace {

    HOOK_INSTALL(HumanoidMoveFSM_LateTick, Velocity_LateTick)
    {
        HOOK_RET_ORIG(HumanoidMoveFSM_LateTick);
        void* __this = HOOK_ARG(HumanoidMoveFSM_LateTick, 0);

        if (!__this)return ret_orig();

        ret_orig();

        if (!self || !self->Active())return;

        auto* rb = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::HumanoidMoveFSM::_rigidbody);
        if (!rb)
            return;

        const auto scale = (self->GetModeIndex() == 0)
        ? Vector3{ self->GetDetailX(), self->GetDetailY(), self->GetDetailZ() }
        : Vector3{ self->GetGlobalScale(), self->GetGlobalScale(), self->GetGlobalScale() };

        UnityUtils::Rigidbody_set_velocity(rb, scale * UnityUtils::Rigidbody_get_velocity(rb));
    }

}

float VelocityFeature::OnGUI(const ImVec2& detailStart, float width)
{
    static const char* modes[] = { XS("Detail"), XS("Global") };

    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiDropdownRow("velocity_mode", XS("Mode"), modes,
        IM_ARRAYSIZE(modes), modeIndex_, ImVec2(x, y), width);

    y += 8.f;

    if (modeIndex_ == 0)
    {
        y += GuiSliderRow("velocity_axis_x", XS("X"), &detailX_, -1.f, 20.f, "%.1f", ImVec2(x, y), width, true);
        y += GuiSliderRow("velocity_axis_y", XS("Y"), &detailY_, -1.f, 20.f, "%.1f", ImVec2(x, y), width, true);
        y += GuiSliderRow("velocity_axis_z", XS("Z"), &detailZ_, -1.f, 20.f, "%.1f", ImVec2(x, y), width, true);
    }
    else
    {
        y += GuiSliderRow("velocity_global_scale", XS("Scale"), &globalScale_, -1.f, 20.f, "%.1f", ImVec2(x, y), width, true);
    }

    y += 10.f;

    y += GuiToggleTextRow("velocity_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void VelocityFeature::OnShutdown()
{
    SetEnabled(false);
}