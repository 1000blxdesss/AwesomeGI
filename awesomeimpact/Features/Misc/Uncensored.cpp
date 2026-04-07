#include "Uncensored.h"
#include "../../GUI/gui.h"
#include "../../Utils/test02.h"

static UncensoredFeature* self{ nullptr };
UncensoredFeature::UncensoredFeature()
    : Feature({ "card_uncensored", XS("Uncensored"), XS("Remove censorship"), "NONE", FeatureTab::Misc })
{
    self = this;
}

float (*get_DitherAlphaValue_Orig)(void*) = nullptr;
float get_DitherAlphaValue_Hook(void* vcBaseSetDitherValue)
{
    if (self && self->Active())
        return 1.f;

    return get_DitherAlphaValue_Orig(vcBaseSetDitherValue);
}

float UncensoredFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiToggleTextRow("uncensored_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 10.f;

    return (y - detailStart.y) + 12.f;
}

void UncensoredFeature::OnShutdown()
{
    SetEnabled(false);
}