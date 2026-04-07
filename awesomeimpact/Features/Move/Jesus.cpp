#include "Jesus.h"
#include "../../GUI/gui.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#include <optional>

static JesusFeature* self{ nullptr };
JesusFeature::JesusFeature()
    : Feature({ "card_jesus", XS("Jesus"), XS("Walk on water"), XS("NONE"), FeatureTab::Move })
{
    self = this;
}

namespace {

    std::optional<uint8_t> g_originalOnWater;

    HOOK_INSTALL(HumanoidMoveFSM_LateTick, Jesus_LateTick)
    {
        HOOK_RET_ORIG(HumanoidMoveFSM_LateTick);
        void* __this = HOOK_ARG(HumanoidMoveFSM_LateTick, 0);

        if (!__this)return ret_orig();

        const auto owner = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::BaseComponentPlugin::_owner);
        if (!owner)return ret_orig();

        const auto baseEntity = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(owner) + offsets::MoleMole::BaseComponent::_entity);
        const auto localAvatar = OtherUtils::AvatarManager();
        if (!baseEntity || !localAvatar || baseEntity != localAvatar) return ret_orig();

        const auto moveData = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::HumanoidMoveFSM::_moveData);
        if (!moveData)return ret_orig();

        auto* onWater = reinterpret_cast<uint8_t*>(moveData) + offsets::MoleMole::VCMoveData::onWater;

        if (self && self->Active())
        {
            if (!g_originalOnWater)
                g_originalOnWater = *onWater;

            *onWater = 1;
        }
        else if (g_originalOnWater)
        {
            *onWater = *g_originalOnWater;
            g_originalOnWater.reset();
        }

        return ret_orig();
    }

}

float JesusFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiToggleTextRow("jesus_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void JesusFeature::OnShutdown()
{
    SetEnabled(false);
}
