#include "ObjectUnlocker.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#include <algorithm>
#include <chrono>
#include <string>

static ObjectUnlockerFeature* self{ nullptr };
ObjectUnlockerFeature::ObjectUnlockerFeature()
    : Feature({ XS("card_auto_challenge"), XS("Object Unlocker"), XS("Activates objects and bla-bla"), XS("NONE"), FeatureTab::Misc })
{
    self = this;
}

namespace {

    inline bool IsTransPoint(EntityType type)
    {
        return type == EntityType::TransPointFirst
            || type == EntityType::TransPointFirstGadget
            || type == EntityType::TransPointSecond
            || type == EntityType::TransPointSecondGadget;
    }

    inline ElementType ElementFromName(const std::string& name)
    {
        struct Mapping { const char* token; ElementType elem; };
        static const Mapping kMap[] = {
            { "_Fire",     ElementType::Fire     },
            { "_Ice",      ElementType::Ice      },
            { "_Electric", ElementType::Electric },
            { "_Water",    ElementType::Water    },
            { "_Grass",    ElementType::Grass    },
            { "_Wind",     ElementType::Wind     },
            { "_Rock",     ElementType::Rock     },
        };
        for (const auto& m : kMap) if (name.find(m.token) != std::string::npos) return m.elem;
        return ElementType::None;
    }

    UPDATE_HOOK(DistanceOpen)
    {
        if (!self || !self->Active()) return;

        static int frameCounter = 0;
        if (++frameCounter < 20) return;
        frameCounter = 0;

        const auto localAvatar = OtherUtils::AvatarManager();
        if (!localAvatar) return;

        auto* mapModuleName = UnityUtils::PtrToStringAnsi((void*)стринги_типо::MapModule());
        auto* scenePointModule = UnityUtils::GetSingletonInstance(UnityUtils::get_SingletonManager(), mapModuleName);
        if (!scenePointModule) return;

        const auto localPos = UnityUtils::GetRelativePosition_Entity(localAvatar);
        const float range = std::clamp(self->GetRange(), 1.f, 1000.f);

        OtherUtils::ForEachEntity([&](void* entity)
            {
                const auto type = static_cast<EntityType>(*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::entityType));
                if (!IsTransPoint(type)) return;

                if (localPos.distance(UnityUtils::GetRelativePosition_Entity(entity)) > range)return;

                auto* lcList = UnityUtils::GetAllLogicComponents(entity);
                auto* lcScenePoint = OtherUtils::FindProp(entity, lcList, стринги_типо::LCScenePoint());
                if (!lcScenePoint)
                {
                    auto* owner = UnityUtils::GetEntityOwner(entity);
                    if (owner && owner != entity)
                    {
                        auto* ownerList = UnityUtils::GetAllLogicComponents(owner);
                        lcScenePoint = OtherUtils::FindProp(owner, ownerList, стринги_типо::LCScenePoint());
                    }
                }
                if (!lcScenePoint) return;

                const bool unlocked = *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(lcScenePoint) + offsets::MoleMole::LCScenePoint::_unlocked);
                if (unlocked) return;

                UnityUtils::MapModule_RequestUnlockTransPoint(scenePointModule, lcScenePoint);
            });
    }

    UPDATE_HOOK(GEAR_LOG)
    {
        if (!self || !self->Enabled()) return;

        static int frameCounter = 0;
        if (++frameCounter < 20) return;
        frameCounter = 0;

        using Clock = std::chrono::steady_clock;
        static auto nextElemTime = Clock::time_point{};

        auto* evtMgrName = UnityUtils::PtrToStringAnsi((void*)стринги_типо::EventManager());
        const auto singletonMgr = UnityUtils::get_SingletonManager();
        if (!singletonMgr) return;
        auto* eventManager = UnityUtils::GetSingletonInstance(singletonMgr, evtMgrName);
        if (!eventManager) return;

        const auto localAvatar = OtherUtils::AvatarManager();
        if (!localAvatar) return;

        const auto localRuntimeID = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(localAvatar) + offsets::MoleMole::BaseEntity::runtimeID);
        if (!localRuntimeID) return;

        const auto localPos = UnityUtils::GetRelativePosition_Entity(localAvatar);
        const float elemRange = std::clamp(self->GetElemRange(), 1.f, 1000.f);

        
        OtherUtils::ForEachEntity([&](void* entity)
            {
                const auto type = static_cast<EntityType>(*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::entityType));
                if (type != EntityType::Gear) return;

                if (localPos.distance(UnityUtils::GetRelativePosition_Entity(entity)) > elemRange)return;

                const auto entityID = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::runtimeID);

                auto* lcList = UnityUtils::GetAllLogicComponents(entity);
                auto* lcAbilityElement = OtherUtils::FindProp(entity, lcList, стринги_типо::LCAbilityElement());
                if (!lcAbilityElement) return;

                auto* lcAbilityState = *reinterpret_cast<void**>(
                    reinterpret_cast<uintptr_t>(lcAbilityElement) + offsets::MoleMole::LCAbilityElement::_lcAbilityState);
                if (!lcAbilityState) return;

                auto* abilityState = reinterpret_cast<AbilityState*>(
                    reinterpret_cast<uintptr_t>(lcAbilityState) + offsets::MoleMole::LCAbilityState::abilityState);
                if (!abilityState || *abilityState != AbilityState::None) return;

                auto* rootObj = UnityUtils::BaseEntity_rootObject(entity);
                if (!rootObj) return;
                auto* nameStr = UnityUtils::Object_get_name(rootObj);
                const auto name = (nameStr && nameStr->length > 0) ? OtherUtils::Utf16ToUtf8(nameStr->chars, nameStr->length) : std::string();
                // ВАХЫВХАВЫХ, А СЕЙЧАС НАЧИНАЕТСЯ ПИЗДОКОСТЫЛЯНИЕ
                const auto elem = ElementFromName(name);
                if (elem == ElementType::None) return;

                const auto curNow = Clock::now();
                if (curNow < nextElemTime) return;

                auto* attackResult = UnityUtils::AttackResult_CreateAttackResult(elem, 1.f);
                if (!attackResult) return;

                auto* cluster = UnityUtils::get_threadSharedCluster();
                if (!cluster) return;

                auto* evtClass = il2cppUtils::GetTypeInfoFromTypeDefinitionIndex(MoleMole_EvtBeingHit);
                if (!evtClass) return;

                auto* evtBeingHit = OtherUtils::AllocateFromPool(cluster, evtClass);
                if (!evtBeingHit) return;

                UnityUtils::AllocateAutoAllocRecycleType(evtBeingHit);
                UnityUtils::EvtBeingHit_Init(evtBeingHit, entityID, localRuntimeID, attackResult, 0u, nullptr);
                UnityUtils::FireEvent(eventManager, evtBeingHit, false);

                nextElemTime = curNow + std::chrono::duration_cast<Clock::duration>(std::chrono::duration<float>(self->GetDelay()));
            });
    }

}

float ObjectUnlockerFeature::OnGUI(const ImVec2& detailStart, float width)
{
    static const char* modes[] = { XS("Waypoint"), XS("ElemMonument") };

    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiDropdownRow("object_unlocker_mode", XS("Mode"), modes,
        IM_ARRAYSIZE(modes), modeIndex_, ImVec2(x, y), width);

    if (modeIndex_ == 0)
    {
        y += GuiSliderRow("object_unlocker_waypoint_range", XS("Range"),
            &rangeW_, 1.f, 45.f, "%.0f", ImVec2(x, y), width, true);
    }
    else
    {
        y += GuiSliderRow("object_unlocker_elem_range", XS("Range"),
            &rangeE_, 1.f, 100.f, "%.0f", ImVec2(x, y), width, true);
        y += GuiSliderRow("object_unlocker_elem_delay", XS("Delay(sec)"),
            &elemDelaySec_, 0.05f, 5.f, "%.2f", ImVec2(x, y), width, true);
    }

    y += 8.f;
    return (y - detailStart.y) + 12.f;
}

void ObjectUnlockerFeature::OnShutdown()
{
    SetEnabled(false);
}
