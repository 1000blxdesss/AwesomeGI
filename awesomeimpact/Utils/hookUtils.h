#pragma once
#include <cstdint>
#include <minwindef.h>
#include <vector>
#include "../globals.h"
#include "../MinHook/include/MinHook.h"
#include "test01.h"


bool HookGameFunction(uint64_t offset, LPVOID detour, void** original, std::vector<void*>& outHooks)
{
    if (!g_GameBase)
        return false;

    void* target = reinterpret_cast<void*>(g_GameBase + offset);
    if (MH_CreateHook(target, detour, original) != MH_OK)
        return false;
    if (MH_EnableHook(target) != MH_OK)
        return false;

    outHooks.push_back(target);
    return true;
}

bool HookApiFunction(const char* moduleName, const char* procName, LPVOID detour, void** original, std::vector<void*>& outHooks)
{
    if (!moduleName || !procName)
        return false;

    HMODULE mod = LI(kernel32, GetModuleHandleA)(moduleName);
    if (!mod)
        return false;
    void* target = reinterpret_cast<void*>(LI(kernel32, GetProcAddress)(mod, procName));
    if (!target)
        return false;

    if (MH_CreateHook(target, detour, original) != MH_OK)
        return false;
    if (MH_EnableHook(target) != MH_OK)
        return false;

    outHooks.push_back(target);
    return true;
}
