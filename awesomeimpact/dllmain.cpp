#include <Windows.h>
#include "core/App.h"
#include "Utils/test01.h"

namespace
{
    App* g_app = nullptr;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        LI(kernel32, DisableThreadLibraryCalls)(hModule);

        static App app(hModule);
        g_app = &app;
        SetGlobalAppInstance(g_app);
        g_app->Bootstrap();
    }
    return TRUE;
}
