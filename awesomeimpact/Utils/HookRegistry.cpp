#include "HookRegistry.h"
#include "../core/App.h"

HOOK_CHAIN_DEFINE(Update, void*, (void* __this), __this)
HOOK_CHAIN_DEFINE(HumanoidMoveFSM_LateTick, void, (void* __this, float deltaTime), __this, deltaTime)
HOOK_CHAIN_DEFINE(ConvertSyncTaskToMotionInfo, void, (void* __this), __this)
HOOK_CHAIN_DEFINE(VCBillboard_UpdateTransform, void, (void* __this, void* cameraTrans), __this, cameraTrans)
HOOK_CHAIN_DEFINE(VCBillboard_NeedHideBillboard, bool, (void* __this), __this)
HOOK_CHAIN_DEFINE(MonoBillboard_SetHPRatio, void, (void* __this, float ratio, bool isLive), __this, ratio, isLive)

HOOK_INSTALL(Update, Update_RunHandlers)
{
    HOOK_RET_ORIG(Update);
    TickGlobalAppFromGameUpdate();
    DrainUnityThreadQueue();
    for (auto fn : GetUpdateHandlers())
        fn(HOOK_ARG(Update, 0));
    return ret_orig();
}

