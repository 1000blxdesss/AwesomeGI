#include "CombatProp.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/test02.h"

#include <cstdlib>
#include <cstring>
#include <array>

static CombatPropFeature* self{ nullptr };
CombatPropFeature::CombatPropFeature()
    : Feature({ "combat_prop", XS("Combat Prop"), XS("Change stats"), XS("NONE"), FeatureTab::Combat })
{
    self = this;
    strcpy_s(valueInput_.data(), valueInput_.size(), "1000");
}

namespace {

    struct PropOption {
        FightPropType type;
        const char* label;
    };

    constexpr int kDataPropOpSet = 1;

    static const PropOption kPropOptions[] = {
        { FightPropType::BASE_HP,           XS("BASE_HP")           },
        { FightPropType::HP,                XS("HP")                },
        { FightPropType::HP_PERCENT,        XS("HP_PERCENT")        },
        { FightPropType::BASE_ATTACK,       XS("BASE_ATTACK")       },
        { FightPropType::ATTACK,            XS("ATTACK")            },
        { FightPropType::ATTACK_PERCENT,    XS("ATTACK_PERCENT")    },
        { FightPropType::BASE_DEFENSE,      XS("BASE_DEFENSE")      },
        { FightPropType::DEFENSE,           XS("DEFENSE")           },
        { FightPropType::DEFENSE_PERCENT,   XS("DEFENSE_PERCENT")   },
        { FightPropType::CRITICAL,          XS("CRITICAL")          },
        { FightPropType::CRITICAL_HURT,     XS("CRITICAL_HURT")     },
        { FightPropType::CHARGE_EFFICIENCY, XS("CHARGE_EFFICIENCY") },
        { FightPropType::ELEMENT_MASTERY,   XS("ELEMENT_MASTERY")   },
        { FightPropType::PHYSICAL_ADD_HURT, XS("PHYSICAL_ADD_HURT") },
        { FightPropType::FIRE_ADD_HURT,     XS("FIRE_ADD_HURT")     },
        { FightPropType::ELEC_ADD_HURT,     XS("ELEC_ADD_HURT")     },
        { FightPropType::WATER_ADD_HURT,    XS("WATER_ADD_HURT")    },
        { FightPropType::GRASS_ADD_HURT,    XS("GRASS_ADD_HURT")    },
        { FightPropType::WIND_ADD_HURT,     XS("WIND_ADD_HURT")     },
        { FightPropType::ROCK_ADD_HURT,     XS("ROCK_ADD_HURT")     },
        { FightPropType::ICE_ADD_HURT,      XS("ICE_ADD_HURT")      },
        { FightPropType::MAX_HP,            XS("MAX_HP")            },
        { FightPropType::CUR_ATTACK,        XS("CUR_ATTACK")        },
        { FightPropType::CUR_DEFENSE,       XS("CUR_DEFENSE")       },
        { FightPropType::CUR_SPEED,         XS("CUR_SPEED")         },
    };

    static const auto kPropCount = static_cast<int>(std::size(kPropOptions));
    static const auto kPropLabels = [] 
        {
            std::array<const char*, kPropCount> out{};
            for (int i = 0; i < kPropCount; ++i)out[i] = kPropOptions[i].label;
            return out;
        }();

    void* FindAvatarCombatComponent()
    {
        const auto playerEntity = OtherUtils::AvatarManager();
        if (!playerEntity) return nullptr;

        const auto lcList = UnityUtils::GetAllLogicComponents(playerEntity);
        if (!lcList || lcList->size <= 0) return nullptr;
        return OtherUtils::FindProp(playerEntity, lcList, стринги_типо::LCAvatarCombat());
    }

    bool TryApplyProp(int propIndex, const char* valueStr)
    {
        if (propIndex < 0 || propIndex >= kPropCount) return false;

        char* end = nullptr;
        const float value = std::strtof(valueStr, &end);
        if (end == valueStr)return false;

        auto* avatarCombat = FindAvatarCombatComponent();
        if (!avatarCombat)return false;

        UnityUtils::UpdateCombatProp(
            avatarCombat,
            static_cast<int>(kPropOptions[propIndex].type),
            value,
            kDataPropOpSet
        );
        return true;
    }

}

float CombatPropFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiDropdownRow(
        "combat_prop_dropdown",
        XS(":O c===3"),
        kPropLabels.data(),
        kPropCount,
        selectedPropIndex_,
        ImVec2(x, y),
        width
    );
    y += 6.f;

    bool confirmed = false;
    y += GuiInputConfirmRow(
        "combat_prop_value_input",
        XS("Value"),
        valueInput_.data(),
        valueInput_.size(),
        confirmed,
        ImVec2(x, y),
        width
    );
    y += 6.f;

    if (confirmed)
        TryApplyProp(selectedPropIndex_, valueInput_.data());

    return (y - detailStart.y) + 12.f;
}

void CombatPropFeature::OnShutdown()
{
    self->SetEnabled(false);
}