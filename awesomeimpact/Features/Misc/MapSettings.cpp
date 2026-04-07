#include "MapSettings.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

static MapSettingsFeature* self{ nullptr };
MapSettingsFeature::MapSettingsFeature()
    : Feature({ "card_map_settings", XS("Map Settings"), XS("Area options"), "NONE", FeatureTab::Misc })
{
    self = this;
    BindConfigBool("map_settings_area_mode", &redZone_);
}

void (*LuaLimitRegionTask_TickInternal_Orig)(void*) = nullptr;
void LuaLimitRegionTask_TickInternal_Hook(void* __this)
{
    if (!self || !self->Active() || !__this)return LuaLimitRegionTask_TickInternal_Orig(__this);
}

float MapSettingsFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiToggleTextRow("map_redzone", XS("RedZone"), XS("Disabled"), redZone_, ImVec2(x, y), width) + 8.f;

    return (y - detailStart.y) + 12.f;
}

void MapSettingsFeature::OnShutdown()
{
    SetEnabled(false);
}