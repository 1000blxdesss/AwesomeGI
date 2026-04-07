#include "OpenChest.h"
#include "../../GUI/gui.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"
#include "../../globals.h"

static OpenChestFeature* self{ nullptr };
OpenChestFeature::OpenChestFeature()
    : Feature({ "card_open_chest", XS("Open Chest"), XS("Automatically open chests"), "NONE", FeatureTab::Misc })
{
    self = this;
}

namespace {

    inline bool Ids(GadgetState state)
    {
        return state == GadgetState::ChestTrap // сундук фембой
            || state == GadgetState::ChestBramble
            || state == GadgetState::ChestFrozen
            || state == GadgetState::ChestRock;
    }

    UPDATE_HOOK(OpenChest)
    {
        if (!self || !self->Active()) return;

        static int frameCounter = 0;
        if (++frameCounter < 30) return;
        frameCounter = 0;

        const auto localPlayer = OtherUtils::AvatarManager();
        if (!localPlayer) return;

        const auto localPos = UnityUtils::GetRelativePosition_Entity(localPlayer);

        auto* itemModuleName = UnityUtils::PtrToStringAnsi((void*)стринги_типо::ItemModule());
        auto* itemModule = UnityUtils::GetSingletonInstance(UnityUtils::get_SingletonManager(), itemModuleName);
        if (!itemModule) return;

        OtherUtils::ForEachEntity([&](void* entity)
            {
                const auto type = static_cast<EntityType>(*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::entityType));
                if (type != EntityType::Chest) return;

                if (localPos.distance(UnityUtils::GetRelativePosition_Entity(entity)) >= self->GetRange()) return;

                auto* lcList = UnityUtils::GetAllLogicComponents(entity);
                auto* lcGadgetMisc = OtherUtils::FindProp(entity, lcList, стринги_типо::LCGadgetMisc());
                if (!lcGadgetMisc) return;

                const auto curState = *reinterpret_cast<GadgetState*>(reinterpret_cast<uintptr_t>(lcGadgetMisc) + offsets::MoleMole::LCGadgetMisc::curGadgetState);

                if (curState == GadgetState::ChestOpened || curState == GadgetState::ChestLocked) return;
                if (Ids(curState))
                    return;

                auto* chestPlugin = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(lcGadgetMisc) + offsets::MoleMole::LCGadgetMisc::_chestPlugin);
                if (chestPlugin && UnityUtils::LCChestPlugin_get_isLock(chestPlugin))
                    return;

                auto* lcGadgetIntee = reinterpret_cast<LCBaseIntee*>(OtherUtils::FindProp(entity, lcList, стринги_типо::LCGadgetIntee()));
                if (lcGadgetIntee)
                {
                    if (lcGadgetIntee->_isInterDisable || lcGadgetIntee->_isLuaInterDisable)
                        return;
                    if (lcGadgetIntee->useInteractionTrigger && !lcGadgetIntee->_interactionTriggerEnter)
                        return;
                }

                const auto runtimeID = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::runtimeID);
                UnityUtils::ItemModule_PickItem(itemModule, runtimeID);
            });
    }

}

float OpenChestFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiSliderRow("chest_range", XS("Range"),
        &range_, 1.f, 100.f, "%.0f", ImVec2(x, y), width, true) + 6.f;

    y += GuiToggleTextRow("chest_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 10.f;

    return (y - detailStart.y) + 12.f;
}

void OpenChestFeature::OnShutdown()
{
    SetEnabled(false);
}
