#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#include <mutex>

namespace {
    std::once_flag g_sceneFlag;
    bool g_confirmCursorUnlocked = false;

    inline void HideConfirmAndLockCursor()
    {
        UnityUtils::ConfirmWithJoypad_Hide();
        UnityUtils::Cursor_set_visible(false);
        UnityUtils::Cursor_set_lockState(1);
        g_confirmCursorUnlocked = false;
    }
}

void (*ActorUtils_OnEnterSceneDone_Orig)() = nullptr;
void ActorUtils_OnEnterSceneDone_Hook()
{
    ActorUtils_OnEnterSceneDone_Orig();
    std::call_once(g_sceneFlag, [] {
        UnityUtils::ConfirmWithJoypad_Show(
            UnityUtils::PtrToStringAnsi((void*)XS("Mihoyo and me will pray that you don't get banned!")),
            UnityUtils::PtrToStringAnsi((void*)XS("Okaay")),
            UnityUtils::PtrToStringAnsi((void*)XS("Nah")),
            nullptr, 0);
        });
}

void (*ConfirmWithJoypad_Awake_Orig)(void*) = nullptr;
void ConfirmWithJoypad_Awake_Hook(void* __this)
{
    ConfirmWithJoypad_Awake_Orig(__this);
    auto* confirm = reinterpret_cast<ConfirmWithJoypad*>(__this);
    if (confirm->acceptButton) UnityUtils::GameObject_SetActive(confirm->acceptButton, false);
    if (confirm->cancelButton) UnityUtils::GameObject_SetActive(confirm->cancelButton, false);
}

void (*ConfirmWithJoypad_Update_Orig)(void*) = nullptr;
void ConfirmWithJoypad_Update_Hook(void* __this)
{
    ConfirmWithJoypad_Update_Orig(__this);

    auto* confirm = reinterpret_cast<ConfirmWithJoypad*>(__this);
    const bool visible = confirm && confirm->dialogObject &&
        UnityUtils::GameObject_get_activeSelf(confirm->dialogObject);

    if (visible)
    {
        if (!g_confirmCursorUnlocked)
        {
            UnityUtils::Cursor_set_visible(true);
            UnityUtils::Cursor_set_lockState(0);
            g_confirmCursorUnlocked = true;
        }
    }
    else if (g_confirmCursorUnlocked)
    {
        UnityUtils::Cursor_set_visible(false);
        UnityUtils::Cursor_set_lockState(1);
        g_confirmCursorUnlocked = false;
    }
}

void (*ConfirmWithJoypad_OnTapCancel_Orig)(void*) = nullptr;
void ConfirmWithJoypad_OnTapCancel_Hook(void* __this)
{
    ConfirmWithJoypad_OnTapCancel_Orig(__this);
    HideConfirmAndLockCursor();
}

void (*ConfirmWithJoypad_OnTapAccept_Orig)(void*) = nullptr;
void ConfirmWithJoypad_OnTapAccept_Hook(void* __this)
{
    ConfirmWithJoypad_OnTapAccept_Orig(__this);
    HideConfirmAndLockCursor();
}
