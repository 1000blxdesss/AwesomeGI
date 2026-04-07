#include "GuiStyle.h"
#include "../../GUI/gui.h"
#include "../../GUI/imgui_internal.h"
#include "../../Utils/test02.h"

GuiStyleFeature::GuiStyleFeature()
    : Feature({ "card_gui_style", XS("GUI"), XS("Customize interface colors"), "NONE", FeatureTab::Visual })
{
}

float GuiStyleFeature::OnGUI(const ImVec2& detailStart, float width)
{
    float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiColorButtonRow("gui_color_window", XS("Window"), GuiColorWindow(), ImVec2(x, y), width) + 6.f;
    y += GuiColorButtonRow("gui_color_child", XS("Child"), GuiColorChild(), ImVec2(x, y), width) + 6.f;
    y += GuiColorButtonRow("gui_color_frame", XS("Frame"), GuiColorFrame(), ImVec2(x, y), width) + 6.f;
    y += GuiColorButtonRow("gui_color_button", XS("Button"), GuiColorButton(), ImVec2(x, y), width) + 6.f;
    y += GuiColorButtonRow("gui_color_text", XS("Text"), GuiColorText(), ImVec2(x, y), width) + 6.f;
    y += GuiColorButtonRow("gui_color_text_enabled", XS("Text Enabled"), GuiColorTextEnabled(), ImVec2(x, y), width) + 6.f;
    y += GuiColorButtonRow("gui_color_text_disabled", XS("Text Disabled"), GuiColorTextDisabled(), ImVec2(x, y), width) + 6.f;
    y += GuiColorButtonRow("gui_color_accent", XS("Accent"), GuiColorAccent(), ImVec2(x, y), width) + 6.f;
    y += GuiColorButtonRow("gui_color_outline", XS("Outline"), GuiColorOutline(), ImVec2(x, y), width) + 6.f;
    y += GuiColorButtonRow("gui_color_highlight", XS("Highlight"), GuiColorHighlight(), ImVec2(x, y), width) + 6.f;
    y += GuiColorButtonRow("gui_color_badge", XS("Badge Capsule"), GuiColorBadge(), ImVec2(x, y), width) + 6.f;
    y += GuiColorButtonRow("gui_color_overlay", XS("Overlay"), GuiOverlayColor(), ImVec2(x, y), width) + 10.f;

    ImVec2 buttonSize(width, 34.f);
    ImVec2 buttonPos(x, y);
    ImGui::SetCursorScreenPos(buttonPos);
    if (ImGui::Button(XS("Reset##gui_colors"), buttonSize))
    {
        GuiResetColors();
    }
    y += buttonSize.y + 8.f;

    GuiApplyStyle();
    return (y - detailStart.y) + 12.f;
}
