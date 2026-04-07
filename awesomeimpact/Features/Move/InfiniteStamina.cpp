#include "InfiniteStamina.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/test02.h"

static InfiniteStaminaFeature* self{ nullptr };
InfiniteStaminaFeature::InfiniteStaminaFeature()
    : Feature({ "card_infinite_stamina", XS("Infinite Stamina"), XS("Never drain stamina"), XS("NONE"), FeatureTab::Move })
{
    self = this;
}

void (*HandleNormalProp_Orig)(void*, uint32_t, int64_t, int) = nullptr;
void HandleNormalProp_Hook(void* dataItem, uint32_t type, int64_t value, int dataPropOp)
{
    if (self && self->Active()
        && (type == PropType::PROP_CUR_PERSIST_STAMINA || type == PropType::PROP_CUR_TEMPORARY_STAMINA)
        && value <= self->GetThreshold())
    {
        value = self->GetRestoreValue();
    }

    HandleNormalProp_Orig(dataItem, type, value, dataPropOp);
}

float InfiniteStaminaFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiSliderRow("stamina_threshold", XS("Threshold"),
        &threshold_, 0.f, 10000.f, "%.0f", ImVec2(x, y), width, true) + 4.f;

    y += GuiSliderRow("stamina_restoreValue", XS("Restore"),
        &restoreValue_, 0.f, 10000.f, "%.0f", ImVec2(x, y), width, true) + 4.f;

    ImGui::SetCursorScreenPos(ImVec2(x, y));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.62f));
    ImGui::TextUnformatted(XS("Stamina Types"));
    ImGui::PopStyleColor();
    y += ImGui::GetFontSize() + 6.f;

    const float checkboxSpacing = ImGui::GetFrameHeight() + 6.f;
    GuiCustomCheckbox("stamina_dive", XS("idk stamina types will be here"), staminaDive_, ImVec2(x, y));
    y += checkboxSpacing;

    y += GuiToggleTextRow("stamina_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void InfiniteStaminaFeature::OnShutdown()
{
    SetEnabled(false);
}