#include "SpawnSomething.h"
#include "../../Utils/HookRegistry.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Logger/Logger.h"
#include "../../Utils/test02.h"

#include <cfloat>
#include <cstdint>
#include <cstdlib>
#include <cstring>

static SpawnSomethingFeature* self{ nullptr };
SpawnSomethingFeature::SpawnSomethingFeature()
    : Feature({ "card_spawn_something", XS("Spawn something"), XS("Spawn idiots by ID"), "NONE", FeatureTab::Misc })
{
    self = this;
}

namespace {

    uint32_t g_entitySerial = 0x70000000u;
    uint64_t g_avatarGuidSerial = 0x7000000000000000ull;

    inline Vector3 GetLocalSpawnPos()
    {
        return UnityUtils::GetAbsolutePosition(
            UnityUtils::GetRelativePosition_Entity(OtherUtils::AvatarManager()));
    }

    void SpawnGadget(int id)
    {
        if (id <= 0) return;
        RunOnUnityThread([id] {
            if (!self || !self->Active()) return;
            if (!OtherUtils::AvatarManager()) return;
            UnityUtils::EntityFactory_CreateLocalGadget(id, GetLocalSpawnPos(), {}, true);
            });
    }

    void SpawnEntity(int configId)
    {
        if (configId <= 0) return;
        RunOnUnityThread([configId] {
            if (!self || !self->Active()) return;
            auto* localAvatar = OtherUtils::AvatarManager();
            if (!localAvatar) return;

            int poseID = 0, aiPose = 0;
            void* aiData = nullptr;
            float scale = 1.f;
            bool isElite = false;
            void* affixList = nullptr;

            const uint32_t templateId = static_cast<uint32_t>(configId);
            {
                // будет вам домашним заданием
                UnityUtils::EntityFactoryMonsterCreationData tpl{};
                if (UnityUtils::EntityFactory_TryGetMonsterCreationData(templateId, &tpl))
                {
                    poseID = tpl.currentPoseID;
                    aiPose = tpl.aiPoseOrState;
                    aiData = tpl.aiData;
                    scale = tpl.scale; //> 0.01f ? tpl.scale : 1.f;
                    isElite = tpl.isElite;
                    affixList = tpl.affixList;
                }
                
            }

            auto* spawnedMonster = UnityUtils::EntityFactory_CreateLocalMonster(
                ++g_entitySerial, static_cast<uint32_t>(configId),
                GetLocalSpawnPos(), {},
                poseID, aiPose, aiData,
                NullNullable<uint32_t>(), scale, false, nullptr,
                isElite, affixList, 0u, false, false);
            });
    }

    void SpawnAvatar(int configId)
    {
        if (configId <= 0) return;
        RunOnUnityThread([configId] {
            if (!self || !self->Active() || !OtherUtils::AvatarManager()) return;
            UnityUtils::EntityFactory_CreateLocalAvatar(
                ++g_avatarGuidSerial, ++g_entitySerial,
                static_cast<uint32_t>(configId), 1u,
                GetLocalSpawnPos(), false, nullptr, 0u);
            });
    }

    void SpawnNpc(int configId)
    {
        if (configId <= 0) return;
        RunOnUnityThread([configId] {
            if (!self || !self->Active() || !OtherUtils::AvatarManager()) return;
            UnityUtils::EntityFactory_CreateNpcEntity(
                ++g_entitySerial, static_cast<uint32_t>(configId),
                GetLocalSpawnPos(), {}, 0u, 0u, false);
            });
    }

}

float SpawnSomethingFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    auto submit = [&](const char* id, const char* label, char* buf, void(*handler)(int)) {
        bool confirmed = false;
        y += GuiInputConfirmRow(id, label, buf, 32, confirmed, ImVec2(x, y), width) + 6.f;
        if (confirmed)
        {
            char* end = nullptr;
            const long val = std::strtol(buf, &end, 10);
            if (end != buf) handler(static_cast<int>(val));
        }
        };

    submit("spawn_gadget_id", XS("Gadget ID"), gadgetIdBuffer_, SpawnGadget);
    submit("spawn_avatar_id", XS("Avatar ID"), avatarIdBuffer_, SpawnAvatar);
    submit("spawn_enemy_id", XS("Enemy ID"), enemyIdBuffer_, SpawnEntity);
    submit("spawn_npc_id", XS("Npc ID"), npcIdBuffer_, SpawnNpc);

    return (y - detailStart.y) + 12.f;
}

void SpawnSomethingFeature::OnShutdown()
{
    SetEnabled(false);
}
