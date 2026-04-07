#include "AutoDialog.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../GUI/imgui_internal.h"

#include <chrono>

static AutoDialogFeature* self{ nullptr };
AutoDialogFeature::AutoDialogFeature()
    : Feature({ "card_auto_dialog", XS("Auto Dialog"), XS("Automate conversation flow"), "NONE", FeatureTab::Misc })
{
    self = this;
}

namespace {
    static void* s_talkDialog = nullptr;
    static bool btnShown = false;

    inline void ForceClickReady(void* talkDialog)
    {
        if (!talkDialog) return;
        auto* base = reinterpret_cast<MonoTalkDialog*>(talkDialog);
        base->_protectTime = 0.0f;
        base->_optionsDisplayDelay = 0.0f;
        base->_waitDialogSelectTime = 0.0f;
        base->_waitCoopSelectTime = 0.0f;
    }

    inline void UpdateAutoDialog()
    {
        if (!self || !self->Active())
            return;
        if (!s_talkDialog) return;

        ForceClickReady(s_talkDialog);

        auto* conversation = reinterpret_cast<MonoGrpConversation*>(UnityUtils::MonoTalkDialog_get_conversationGrp(s_talkDialog));
        if (conversation)
        {
            UnityUtils::MonoGrpConversation_ForceShowAllText(conversation);
            void* typewriter = conversation->_typewriter;
            if (typewriter) UnityUtils::MonoTypewriter_ShowAllText(typewriter, true);
        }

        if (self->AutoSelectEnabled())
        {
            void* selectGrp = UnityUtils::MonoTalkDialog_get_selectGrp(s_talkDialog);
            if (selectGrp)
            {
                const int count = UnityUtils::MonoGrpSelect_get_itemCount(selectGrp);
                if (count > 0)
                {
                    UnityUtils::MonoGrpSelect_set_CurrSelection(selectGrp, 0);
                    UnityUtils::MonoGrpSelect_ConfirmSelection(selectGrp);
                }
            }
        }

        if (self->AutoSkipEnabled())
        {
            ForceClickReady(s_talkDialog);
            UnityUtils::MonoTalkDialog_set_showClickTip(s_talkDialog, true);
            UnityUtils::MonoTalkDialog_OnDialogClick(s_talkDialog, true);
        }
    }
}

void (*CutScene_ClearView_Orig)(void*) = nullptr;
void CutScene_ClearView_Hook(void* __this)
{
    btnShown = false;
    if (CutScene_ClearView_Orig)
        CutScene_ClearView_Orig(__this);
}

void (*MonoTalkDialog_OnEnable_Orig)(void*) = nullptr;
void MonoTalkDialog_OnEnable_Hook(void* __this)
{
    if (MonoTalkDialog_OnEnable_Orig)
        MonoTalkDialog_OnEnable_Orig(__this);

    if (!self || !self->Active())
        return;

    s_talkDialog = __this;
}

void (*MonoTalkDialog_HideDialog_Orig)(void*, bool) = nullptr;
void MonoTalkDialog_HideDialog_Hook(void* __this, bool hide)
{
    if (hide && s_talkDialog == __this)
        s_talkDialog = nullptr;

    if (MonoTalkDialog_HideDialog_Orig)
        MonoTalkDialog_HideDialog_Orig(__this, hide);
}

void (*InLevelCutScenePageContext_UpdateView_Orig)(void*) = nullptr;
void InLevelCutScenePageContext_UpdateView_Hook(void* __this)
{
    g_loginDoorOverlayActive = false;

    if (InLevelCutScenePageContext_UpdateView_Orig)
        InLevelCutScenePageContext_UpdateView_Orig(__this);

    if (!btnShown)
    {
        btnShown = true;
        UnityUtils::InLevelCutScenePageContext_RealShowSkipBtn(__this, true);
    }

    if (!self || !self->Active())
        return;

    UpdateAutoDialog();

    if (self->AutoSkipEnabled()) UnityUtils::InLevelCutScenePageContext_Skip(__this);
}

float AutoDialogFeature::OnGUI(const ImVec2& detailStart, float width)
{
    float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    

    float selectRow = GuiToggleTextRow("dialog_auto_select", XS("Auto Select"), XS("Enabled"), autoSelect_, ImVec2(x, y), width);
    y += selectRow + 6.f;

    float skipRow = GuiToggleTextRow("dialog_auto_skip", XS("Auto Skip"), XS("Enabled"), autoSkip_, ImVec2(x, y), width);
    y += skipRow + 6.f;

    float holdRow = GuiToggleTextRow("dialog_hold", XS("Key"), XS("Hold"), hold_, ImVec2(x, y), width);
    y += holdRow + 6.f;

    return (y - detailStart.y) + 12.f;
}

void AutoDialogFeature::OnShutdown() {
    SetEnabled(false);
    s_talkDialog = nullptr;
}
