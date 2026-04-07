#include "NameTags.h"
#include "../../GUI/gui.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"
#include "../../globals.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>

extern ImFont* g_fontNameTags;

static NameTagsFeature* self{ nullptr };
NameTagsFeature::NameTagsFeature()
    : Feature({ "card_name_tags", XS("Name Tags"), XS("Customize overhead info"), "NONE", FeatureTab::Visual })
{
    self = this;
}

namespace {

    std::unordered_map<void*, float> g_hpRatio;

    inline void SetScaleIfNeeded(void* component, float scale)
    {
        if (!component || scale <= 0.01f || scale == 1.f) return;
        auto* tr = UnityUtils::Component_get_transform(component);
        if (tr) UnityUtils::Transform_set_localScale(tr, { scale, scale, scale });
    }

    inline void ActivateComponent(void* component)
    {
        if (!component) return;
        auto* go = UnityUtils::Component_get_gameObject(component);
        if (go) UnityUtils::GameObject_SetActive(go, true);
    }

    bool ApplyToBillboard(void* monoBillboard, float ratio)
    {
        if (!monoBillboard) return false;

        auto* cb = reinterpret_cast<MonoCombatBillboard*>(
            UnityUtils::MonoBillboard_get_combatBillboard(monoBillboard));
        if (!cb) return false;

        const float scale = self->GetScale();
        const float textScale = scale * self->GetTextScale();

        UnityUtils::Behaviour_set_enabled(cb, true);
        ActivateComponent(cb);

        UnityUtils::MonoCombatBillboard_set_showTitleGrp(cb, true);
        UnityUtils::MonoCombatBillboard_set_showName(cb, true);
        UnityUtils::MonoCombatBillboard_set_showOnlineID(cb, false);
        UnityUtils::MonoCombatBillboard_set_showCombatBars(cb, true);
        UnityUtils::MonoCombatBillboard_set_showHpGrp(cb, true);
        UnityUtils::MonoCombatBillboard_set_showLevel(cb, true);
        UnityUtils::MonoCombatBillboard_SetLevelDisplay(cb, 1.f);
        UnityUtils::MonoCombatBillboard_SetScale(cb, scale);

        if (cb->_combatBars) { ActivateComponent(cb->_combatBars); SetScaleIfNeeded(cb->_combatBars, scale); }
        if (cb->_titleGrp) { ActivateComponent(cb->_titleGrp);   SetScaleIfNeeded(cb->_titleGrp, textScale); }
        if (cb->_hpBar) { ActivateComponent(cb->_hpBar);      SetScaleIfNeeded(cb->_hpBar, scale); }

        if (cb->_nameText)
        {
            auto* s = UnityUtils::Text_get_text(cb->_nameText);
            if (s && s->length > 0)
            {
                auto text = OtherUtils::Utf16ToUtf8(s->chars, s->length);

                const auto pct = text.rfind('%');
                if (pct != std::string::npos) {
                    const auto sp = text.rfind(' ', pct);
                    if (sp != std::string::npos) text = text.substr(0, sp);
                }

                if (!text.empty() && text.back() != '.') text += '.';

                if (self->ShowHp())
                    text += " " + std::to_string(std::clamp(static_cast<int>(ratio * 100.f + 0.5f), 0, 999)) + "%";

                UnityUtils::Text_set_text(cb->_nameText, UnityUtils::PtrToStringAnsi((void*)text.c_str()));
            }
        }

        if (cb->_onlineIdText)
        {
            auto* empty = UnityUtils::PtrToStringAnsi((void*)"");
            UnityUtils::Text_set_text(cb->_onlineIdText, empty);
        }

        return true;
    }

    HOOK_INSTALL(VCBillboard_UpdateTransform, NameTags_VCBillboard_UpdateTransform)
    {
        HOOK_RET_ORIG(VCBillboard_UpdateTransform);
        void* __this = HOOK_ARG(VCBillboard_UpdateTransform, 0);
        ret_orig();

        if (!self || !self->Active() || !__this) return;

        *reinterpret_cast<bool*>(
            reinterpret_cast<uintptr_t>(__this) + offsets::MoleMole::VCBillboard::_forceAddCombat) = true;

        auto* bb = UnityUtils::VCBillboard_get_billboard(__this);
        if (!bb) return;

        if (!UnityUtils::MonoBillboard_get_combatBillboard(bb))
        {
            auto* empty = UnityUtils::PtrToStringAnsi((void*)"");
            UnityUtils::MonoBillboard_CreateCombatBillboard(bb, 0, nullptr, empty);
        }

        const auto it = g_hpRatio.find(bb);
        ApplyToBillboard(bb, it != g_hpRatio.end() ? it->second : 1.f);
    }

    HOOK_INSTALL(MonoBillboard_SetHPRatio, NameTags_MonoBillboard_SetHPRatio)
    {
        HOOK_RET_ORIG(MonoBillboard_SetHPRatio);
        void* __this = HOOK_ARG(MonoBillboard_SetHPRatio, 0);
        float ratio = HOOK_ARG(MonoBillboard_SetHPRatio, 1);
        ret_orig();

        if (!self || !self->Active() || !__this) return;
        g_hpRatio[__this] = ratio;
        ApplyToBillboard(__this, ratio);
    }

    HOOK_INSTALL(VCBillboard_NeedHideBillboard, NameTags_VCBillboard_NeedHideBillboard)
    {
        HOOK_RET_ORIG(VCBillboard_NeedHideBillboard);
        void* __this = HOOK_ARG(VCBillboard_NeedHideBillboard, 0);

        if (!self || !self->Active() || !__this)
            return ret_orig();

        return false;
    }

}

float NameTagsFeature::OnGUI(const ImVec2& detailStart, float width)
{
    static const char* targets[] = {
        XS("All"),         XS("Avatar"),       XS("Monster"),      XS("Bullet"),
        XS("AttackPhyisicalUnit"), XS("AOE"), XS("Camera"),       XS("EnviroArea"),
        XS("Equip"),       XS("MonsterEquip"), XS("Grass"),        XS("Level"),
        XS("NPC"),         XS("TransPointFirst"), XS("TransPointFirstGadget"),
        XS("TransPointSecond"), XS("TransPointSecondGadget"),
        XS("DropItem"),    XS("Field"),        XS("Gadget"),       XS("Water"),
        XS("GatherPoint"), XS("GatherObject"), XS("AirflowField"), XS("SpeedupField"),
        XS("Gear"),        XS("Chest"),        XS("EnergyBall"),   XS("ElemCrystal"),
        XS("Timeline"),    XS("Worktop"),      XS("Team"),         XS("Platform"),
        XS("AmberWind"),   XS("EnvAnimal"),    XS("SealGadget"),   XS("Tree"),
        XS("Bush"),        XS("QuestGadget"),  XS("Lightning"),    XS("RewardPoint"),
        XS("RewardStatue"), XS("MPLevel"),     XS("WindSeed"),     XS("MpPlayRewardPoint"),
        XS("ViewPoint"),   XS("RemoteAvatar"), XS("GeneralRewardPoint"), XS("PlayTeam"),
        XS("OfferingGadget"), XS("EyePoint"), XS("MiracleRing"), XS("Foundation"),
        XS("WidgetGadget"), XS("Vehicle"),    XS("DangerZone"),   XS("EchoShell"),
        XS("HomeGatherObject"), XS("Projector"), XS("Screen"),   XS("CustomTile"),
        XS("FishPool"),    XS("FishRod"),      XS("CustomGadget"),
        XS("RoguelikeOperatorGadget"), XS("ActivityInteractGadget"), XS("BlackMud"),
        XS("SubEquip"),    XS("UIInteractGadget"), XS("NightCrowGadget"), XS("Partner"),
        XS("DeshretObeliskGadget"), XS("CoinCollectLevelGadget"), XS("UgcSpecialGadget"),
        XS("UgcTowerLevelUpGadget"), XS("JourneyGearOperatorGadget"),
        XS("CurveMoveGadget"), XS("MagnetPlant"), XS("PlaceHolder"),
    };

    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiDropdownRow("card_name_tags_target", XS("Target"), targets,
        IM_ARRAYSIZE(targets), targetIndex_, ImVec2(x, y), width) + 10.f;

    y += GuiSliderRow("card_name_tags_scale", XS("Scale"),
        &scale_, 0.5f, 3.f, "%.2f", ImVec2(x, y), width, true) + 6.f;

    y += GuiSliderRow("card_name_tags_text_scale", XS("Text Scale"),
        &textScale_, 0.5f, 3.f, "%.2f", ImVec2(x, y), width, true) + 10.f;

    y += GuiToggleTextRow("card_name_tags_show_obj", XS("Show Obj Name"),
        XS("Enabled"), showObjName_, ImVec2(x, y), width) + 6.f;

    y += GuiToggleTextRow("card_name_tags_show_hp", XS("Show HP"),
        XS("Enabled"), showHp_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void NameTagsFeature::OnDraw()
{
    if (!self->Active() || !showObjName_) return;

    auto* camera = UnityUtils::GetMainCamera();
    if (!camera) return;

    auto* dl = ImGui::GetForegroundDrawList();
    if (!dl) return;

    auto* font = g_fontNameTags ? g_fontNameTags : ImGui::GetFont();
    const float fontSize = font ? font->LegacySize : ImGui::GetFontSize();
    const int screenH = UnityUtils::get_height();

    OtherUtils::ForEachEntity([&](void* entity)
        {
            if (targetIndex_ > 0)
            {
                const auto type = *reinterpret_cast<EntityType*>(
                    reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::entityType);
                if (static_cast<int>(type) != targetIndex_) return;
            }

            auto* rootObj = UnityUtils::BaseEntity_rootObject(entity);
            if (!rootObj) return;

            auto* nameStr = UnityUtils::Object_get_name(rootObj);
            std::string name = (nameStr && nameStr->length > 0)
                ? OtherUtils::Utf16ToUtf8(nameStr->chars, nameStr->length)
                : std::string(XS("Unknown"));
            if (name.empty()) name = XS("Unknown");

            auto world = UnityUtils::GetRelativePosition_Entity(entity);
            if (world.zero()) return;
            world.y -= 0.35f;

            const auto screen = OtherUtils::WorldToScreenPointCorrected(camera, world);
            if (screen.z <= 1.f) return;

            const ImVec2 pos = { screen.x, static_cast<float>(screenH) - screen.y };
            const auto textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.f, name.c_str());
            const ImVec2 textPos = {
                std::floor(pos.x - textSize.x * 0.5f + 0.5f),
                std::floor(pos.y + 0.5f)
            };

            dl->AddText(font, fontSize, textPos, IM_COL32(255, 255, 255, 255), name.c_str());
        });
}

void NameTagsFeature::OnShutdown()
{
    SetEnabled(false);
}
