#include "UnlockStatues.h"
#include "AutoDialog.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"

#include <chrono>
#include <cctype>
#include <cstring>
#include <string>
#include <unordered_map>
#include "../../Utils/test02.h"

UnlockStatuesFeature* self{ nullptr };
UnlockStatuesFeature::UnlockStatuesFeature()
    : Feature({ "card_unlock_statues", XS("Unlock Statues"), XS("waiting proto..."), "NONE", FeatureTab::Misc })
{
    self = this;
}

void UnlockStatuesFeature::OnEnable() {
	self->SetEnabled(false);
}

namespace {
    using Clock = std::chrono::steady_clock;
    static std::unordered_map<uint32_t, Clock::time_point> g_lastTry;

   

    UPDATE_HOOK(UnlockStatues_Update)
    {
      
    }
}

float UnlockStatuesFeature::OnGUI(const ImVec2& detailStart, float width)
{
    float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;
    y += GuiSliderRow("unlock_statues_range", XS("Range"), &range_, 1.f, 45.f, "%.0f", ImVec2(x, y), width, true);
    y += 8.f;
    return (y - detailStart.y) + 12.f;
}

void UnlockStatuesFeature::OnShutdown()
{
    g_lastTry.clear();
    self->SetEnabled(false);
}
