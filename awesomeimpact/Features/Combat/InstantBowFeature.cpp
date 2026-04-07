#include "InstantBowFeature.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Logger/Logger.h"
#include "../../Utils/test02.h"

static InstantBowFeature* self{ nullptr };
InstantBowFeature::InstantBowFeature()
	: Feature({ "card_instant_bow", XS("Instant Bow"), XS("Shoot...instantly"), XS("NONE"), FeatureTab::Combat })
{
	self = this;
}

// используется, потому что дает забавный баг на путешественнике
void (*SetDynamicFloatWithRange_Orig)(void*, uintptr_t, float, float, float, bool, bool) = nullptr;
void SetDynamicFloatWithRange_Hook(void* __this, uintptr_t key, float value, float min, float max, bool NKEACNMPLBI, bool CHJAJLJLMML)
{
	if (!self || !self->Active() || !__this) return SetDynamicFloatWithRange_Orig(__this, key, value, min, max, NKEACNMPLBI, CHJAJLJLMML);
	SetDynamicFloatWithRange_Orig(__this, key, max, min, max, NKEACNMPLBI, CHJAJLJLMML);
}

float InstantBowFeature::OnGUI(const ImVec2& detailStart, float width)
{
	float x = detailStart.x + 20.f;
	float y = detailStart.y + 12.f;
	float holdRow = GuiToggleTextRow("instant_bow_hold", XS("Key"), XS("Hold"), hold_, ImVec2(x, y), width);
	y += holdRow + 6.f;
	return (y - detailStart.y) + 12.f;
}
\
\
void InstantBowFeature::OnShutdown()
{
	self->SetEnabled(false);
}