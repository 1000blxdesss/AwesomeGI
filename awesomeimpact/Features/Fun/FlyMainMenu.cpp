#include "FlyMainMenu.h"
#include "../Misc/FpsUnlock.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#include <cstdlib>
#include <Windows.h>

namespace {

    Vector3 g_cameraPos{};
    float   g_yaw = 0.f;
    float   g_pitch = 0.f;
    POINT   g_lastMouse{};

    constexpr float kMoveSpeed = 0.5f;
    constexpr float kMouseSpeed = 0.15f;

    const char* kTips[] = {
        XS("That some functions can be expanded by right-clicking"),
        XS("That you can fly in the main menu"),
        XS("That ESP can render objects by partial name match"),
        XS("That ESP uses names from Name Tags to filter entities"),
    };

}

void (*MonoLoginScene_Update_Orig)(void*) = nullptr;
void MonoLoginScene_Update_Hook(void* __this)
{
    MonoLoginScene_Update_Orig(__this);
    auto* scene = reinterpret_cast<MonoLoginScene*>(__this);

    auto* camTr = scene ? scene->cameraTrans : nullptr;
    if (!camTr) return;

    g_cameraPos = UnityUtils::Transform_get_position(camTr);

    static bool wasRmb = false;
    const bool rmb = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    POINT mouse{};
    GetCursorPos(&mouse);

    if (rmb)
    {
        if (!wasRmb)
        {
            g_lastMouse = mouse;
            const auto e = UnityUtils::Transform_get_localEulerAngles(camTr);
            g_pitch = e.x;
            g_yaw = e.y;
        }
        else
        {
            g_yaw += (mouse.x - g_lastMouse.x) * kMouseSpeed;
            g_pitch += (mouse.y - g_lastMouse.y) * kMouseSpeed;
            UnityUtils::Transform_set_localEulerAngles(camTr, { g_pitch, g_yaw, 0.f });
            g_lastMouse = mouse;
        }
    }
    else
    {
        g_lastMouse = mouse;
    }
    wasRmb = rmb;

    const auto fwd = UnityUtils::Transform_get_forward(camTr);
    const auto right = UnityUtils::Transform_get_right(camTr);

    if (GetAsyncKeyState('W') & 0x8000) { g_cameraPos.x += fwd.x * kMoveSpeed; g_cameraPos.y += fwd.y * kMoveSpeed; g_cameraPos.z += fwd.z * kMoveSpeed; }
    if (GetAsyncKeyState('S') & 0x8000) { g_cameraPos.x -= fwd.x * kMoveSpeed; g_cameraPos.y -= fwd.y * kMoveSpeed; g_cameraPos.z -= fwd.z * kMoveSpeed; }
    if (GetAsyncKeyState('A') & 0x8000) { g_cameraPos.x -= right.x * kMoveSpeed; g_cameraPos.z -= right.z * kMoveSpeed; }
    if (GetAsyncKeyState('D') & 0x8000) { g_cameraPos.x += right.x * kMoveSpeed; g_cameraPos.z += right.z * kMoveSpeed; }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)  g_cameraPos.y += kMoveSpeed;
    if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) g_cameraPos.y -= kMoveSpeed;

    UnityUtils::Transform_set_position(camTr, g_cameraPos);
}

void (*MonoLoginScene_OpenDoor_Orig)(void*, void*) = nullptr;
void MonoLoginScene_OpenDoor_Hook(void* __this, void* callback)
{
    g_loginDoorOverlayActive = true;
    MonoLoginScene_OpenDoor_Orig(__this, callback);
}

void (*MonoLoadingCanvas_Start_Orig)(void*) = nullptr;
void MonoLoadingCanvas_Start_Hook(void* __this)
{
    g_loginDoorOverlayActive = false;
    MonoLoadingCanvas_Start_Orig(__this);

    auto* canvas = reinterpret_cast<MonoLoadingCanvas*>(__this);

    auto* bgImg = UnityUtils::MonoLoadingCanvas_get_bgImg(__this);
    if (bgImg)
        UnityUtils::Graphic_set_color(bgImg, { 0.f, 0.f, 0.f, 1.f });

    auto setText = [](void* text, const char* str) {
        if (text) UnityUtils::Text_set_text(text, UnityUtils::PtrToStringAnsi((void*)str));
        };

    setText(canvas->_tipsText, XS("NEVERSTOPWALKING"));
    setText(canvas->_txtTipsTitle, XS("Did you know..."));
    setText(canvas->_txtTipsContent, kTips[std::rand() % std::size(kTips)]);
}
