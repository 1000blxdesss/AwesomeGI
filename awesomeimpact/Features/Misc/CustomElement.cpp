#include "CustomElement.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/test02.h"

#include <algorithm>

static CustomElementFeature* self{ nullptr };
CustomElementFeature::CustomElementFeature()
    : Feature({ "card_custom_element", XS("Custom Element"), XS("Override aura element"), XS("NONE"), FeatureTab::Misc })
{
    self = this;
}
namespace {

    constexpr ElementType kElementValues[] = {
        ElementType::None, ElementType::Fire, ElementType::Water, ElementType::Grass,
        ElementType::Electric, ElementType::Ice, ElementType::Wind, ElementType::Rock,
    };

    const char* kElementOptions[] = {
        XS("Normal"), XS("Pyro"), XS("Hydro"), XS("Dendro"),
        XS("Electro"), XS("Cryo"), XS("Anemo"), XS("Geo"),
    };

    inline int ClampElementIndex(int index)
    {
        return std::clamp(index, 0, IM_ARRAYSIZE(kElementValues) - 1);
    }

    inline bool IsLocalPlayer(uint32_t runtimeID)
    {
        const auto playerEntity = OtherUtils::AvatarManager();
        if (!playerEntity) return false;

        return runtimeID == *reinterpret_cast<uint32_t*>(
            reinterpret_cast<uintptr_t>(playerEntity) + offsets::MoleMole::BaseEntity::runtimeID);
    }

}



int CustomElementFeature::GetElementValue() const
{
    return static_cast<int>(kElementValues[ClampElementIndex(elementIndex_)]);
}

float CustomElementFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    elementIndex_ = ClampElementIndex(elementIndex_);

    y += GuiDropdownRow("custom_element_type", XS("Element"),
        kElementOptions, IM_ARRAYSIZE(kElementOptions),
        elementIndex_, ImVec2(x, y), width) + 6.f;

    y += GuiToggleTextRow("custom_element_override_durability", XS("Override Aura"),
        XS("Enabled"), overrideDurability_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void CustomElementFeature::OnShutdown()
{
    SetEnabled(false);
}

void CustomElement_OnFireBeingHit(uint32_t attackeeRuntimeID, void* attackResult)
{
    if (!self || !self->Active() || !attackResult) return;
    if (IsLocalPlayer(attackeeRuntimeID)) return;
    UnityUtils::AttackResult_set_elementType(attackResult, self->GetElementValue(), nullptr);
}