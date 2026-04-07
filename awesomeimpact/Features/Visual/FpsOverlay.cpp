#include "FpsOverlay.h"
#include "../../GUI/gui.h"
#include "../../Utils/test02.h"

static FpsOverlayFeature* self{ nullptr };
FpsOverlayFeature::FpsOverlayFeature()
    : Feature({ "card_fps_overlay", XS("FPS Overlay"), XS("Display frame counter"), "NONE", FeatureTab::Visual })
{
    self = this;
}

float FpsOverlayFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiToggleTextRow("fps_overlay_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void FpsOverlayFeature::OnDraw()
{
    if (!self || !self->Active()) return;

    auto* dl = ImGui::GetForegroundDrawList();
    if (!dl) return;

    char buf[32];
    _snprintf_s(buf, sizeof(buf), _TRUNCATE, XS("FPS: %.0f"), ImGui::GetIO().Framerate);

    const auto textSize = ImGui::CalcTextSize(buf);
    constexpr ImVec2 kPad = { 8.f, 6.f };
    constexpr ImVec2 kPos = { 12.f, 12.f };

    const ImVec2 bgMax = { kPos.x + textSize.x + kPad.x * 2.f, kPos.y + textSize.y + kPad.y * 2.f };
    const ImVec2 textPos = { kPos.x + kPad.x, kPos.y + kPad.y };

    dl->AddRectFilled(kPos, bgMax, IM_COL32(0, 0, 0, 255), 6.f);
    dl->AddText({ textPos.x + 1.f, textPos.y + 1.f }, IM_COL32(0, 0, 0, 200), buf);
    dl->AddText(textPos, IM_COL32(255, 255, 255, 240), buf);
}

void FpsOverlayFeature::OnShutdown()
{
    SetEnabled(false);
}