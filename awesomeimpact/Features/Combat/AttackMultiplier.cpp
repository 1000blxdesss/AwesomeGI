#include "AttackMultiplier.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../Misc/CustomElement.h"
#include <random>
#include "../../Logger/Logger.h"
#include "../../Utils/test02.h"

static AttackMultiplierFeature* self{ nullptr };
AttackMultiplierFeature::AttackMultiplierFeature()
    : Feature({ "card_attack_multiplier", XS("Attack Multiplier"), XS("meow"), XS("NONE"), FeatureTab::Combat })
{self = this;}

void (*FireBeingHitEvent_Orig)(void*, uint32_t, void*) = nullptr;
void FireBeingHitEvent_Hook(void* __this, uint32_t attackeeRuntimeID, void* attackResult)
{
    CustomElement_OnFireBeingHit(attackeeRuntimeID, attackResult);
    if (!self || !self->Active() || !__this) return FireBeingHitEvent_Orig(__this, attackeeRuntimeID, attackResult);

    if (const auto playerEntity = OtherUtils::AvatarManager())
    {
        //// or read _CurAvatarEntityId
        const auto playerRuntimeID = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(playerEntity) + offsets::MoleMole::BaseEntity::runtimeID);
        if (attackeeRuntimeID == playerRuntimeID) return FireBeingHitEvent_Orig(__this, attackeeRuntimeID, attackResult);
    }
    const auto multiplier = static_cast<int>(self->GetMultiplier());

    for (int i = 1; i < multiplier; ++i) FireBeingHitEvent_Orig(__this, attackeeRuntimeID, attackResult);
}

float AttackMultiplierFeature::OnGUI(const ImVec2& detailStart, float width)
{
    float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    float rowHeight = GuiSliderRow("attack_multiplier", XS("Multiplier"), &multiplier_, 1.f, 100.f, XS("%.0f x"), ImVec2(x, y), width);
    y += rowHeight + 10.f;

    float holdRow = GuiToggleTextRow("attack_multiplier_hold", XS("Key"), XS("Hold"), hold_, ImVec2(x, y), width);
    y += holdRow + 10.f;
    return (y - detailStart.y) + 12.f;
}


void AttackMultiplierFeature::OnShutdown() 
{
    self->SetEnabled(false);
}
