#include "SprintDelay.h"
#include "../../GUI/gui.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

static SprintDelayFeature* self{ nullptr };
SprintDelayFeature::SprintDelayFeature()
    : Feature({ "card_sprint_delay", XS("Sprint Delay"), XS("Remove delay? idk"), XS("NONE"), FeatureTab::Move })
{
    self = this;
}

namespace {

    HOOK_INSTALL(HumanoidMoveFSM_LateTick, SprintDelay_LateTick)
    {
        HOOK_RET_ORIG(HumanoidMoveFSM_LateTick);
        void* __this = HOOK_ARG(HumanoidMoveFSM_LateTick, 0);
        const float deltaTime = HOOK_ARG(HumanoidMoveFSM_LateTick, 1);

        ret_orig();

        if (!self || !self->Active() || !__this || !g_GameBase) return;

        const auto fn = reinterpret_cast<void(*)(void*, float)>(g_GameBase + offsets::MoleMole::HumanoidMoveFSM::UpdateSprintCooldown);
        if (fn)fn(__this, deltaTime + 0.5f);
    }

}

float SprintDelayFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    ImGui::SetCursorScreenPos(ImVec2(x, y));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.58f));
    ImGui::TextUnformatted(XS(":3"));
    ImGui::PopStyleColor();
    y += ImGui::GetFontSize() + 12.f;

    y += GuiToggleTextRow("sprint_delay_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void SprintDelayFeature::OnShutdown()
{
    SetEnabled(false);
}