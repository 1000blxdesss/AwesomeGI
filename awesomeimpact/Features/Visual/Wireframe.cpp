#include "Wireframe.h"
#include "../../GUI/gui.h"
#include "../../Utils/test02.h"

WireframeFeature::WireframeFeature()
    : Feature({ "card_wireframe", XS("Wireframe"), XS("Polygon overlay"), "NONE", FeatureTab::Visual })
{
}

// удалено из-за проблем с совместимостью

float WireframeFeature::OnGUI(const ImVec2& detailStart, float width)
{
    float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    /*float colorRow = GuiColorButtonRow("wireframe_color", "Color", color_, ImVec2(x, y), width, false);
    y += colorRow + 8.f;*/

    float toggleRow = GuiToggleTextRow("wireframe_hold", XS("Key"), XS("Hold"), hold_, ImVec2(x, y), width);
    y += toggleRow + 10.f;

    /*float globalRow = GuiToggleTextRow("wireframe_global", "Global", "Enabled", global_, ImVec2(x, y), width);
    y += globalRow + 8.f;

    float aggrRow = GuiToggleTextRow("wireframe_aggr", "Aggressive", "Enabled", aggressive_, ImVec2(x, y), width);
    y += aggrRow + 10.f;*/

    return (y - detailStart.y) + 12.f;
}
