#include "FriendlyFire.h"
#include "../../GUI/gui.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

static FriendlyFireFeature* self{ nullptr };
FriendlyFireFeature::FriendlyFireFeature()
    : Feature({ "card_friendly_fire", XS("Friendly Fire"), XS("Damage in CO-OP"), XS("NONE"), FeatureTab::Combat })
{
	self = this;
}

uint32_t (*__Orig)(int32_t, int32_t, bool);
uint32_t __Hook(int32_t attackType, int32_t subType, bool applyDamage) 
{
    if (!self || !self->Active())return __Orig(attackType, subType, applyDamage); return 1;
}

float FriendlyFireFeature::OnGUI(const ImVec2& detailStart, float width)
{
    float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    float holdRow = GuiToggleTextRow("friendly_fire_hold", XS("Key"), XS("Hold"), hold_, ImVec2(x, y), width);
    y += holdRow + 6.f;
    return (y - detailStart.y) + 12.f;
}

void FriendlyFireFeature::OnShutdown()
{
	self->SetEnabled(false);
}
