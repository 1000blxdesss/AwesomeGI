#include "App.h"
#include <cstdio>
#include <locale>
#include <string>
#include "../Logger/Logger.h"
#include "../globals.h"
#include "../Utils/test02.h"
#include "../MinHook/include/MinHook.h"
#include "../GUI/kiero.h"
#include <d3d11.h>
#include "../Utils/ComPtr.h"
#include "../GUI/imgui_impl_dx11.h"
#include "../GUI/imgui_impl_win32.h"
#include "../GUI/gui.h"
#include "../Utils/hookUtils.h"
#include "../Features/FeatureBase.h"
#include "../Features/Combat/GodMode.h"
#include "../Features/Combat/KillAura.h"
#include "../Features/Combat/InfiniteSkills.h"
#include "../Features/Combat/VoidAura.h"
#include "../Features/Combat/AttackMultiplier.h"
#include "../Features/Combat/FriendlyFire.h"
#include "../Features/Move/Velocity.h"
#include "../Features/Move/InfiniteStamina.h"
#include "../Features/Move/SprintDelay.h"
#include "../Features/Move/NoClip.h"
#include "../Features/Move/Jesus.h"
#include "../Features/Move/MapTp.h"
#include "../Features/Move/CustomTp.h"
#include "../Features/Move/AirJump.h"
#include "../Features/Move/Spin.h"
#include "../Features/Move/BowTp.h"
#include "../Features/Visual/Esp.h"
#include "../Features/Visual/SkeletonEsp.h"
#include "../Features/Visual/JumpCircle.h"
#include "../Features/Visual/CustomEffect.h"
#include "../Features/Visual/Chams.h"
#include "../Features/Visual/Wireframe.h"
#include "../Features/Visual/GuiStyle.h"
#include "../Features/Visual/ArrayList.h"
#include "../Features/Visual/FpsOverlay.h"
#include "../Features/Visual/Atmosphere.h"
#include "../Features/Visual/CameraSettings.h"
#include "../Features/Visual/NameTags.h"
#include "../Features/Visual/TextureChanger.h"
#include "../Features/Visual/ProfileChanger.h"
#include "../Features/Visual/SkinChangerFeature.h"
#include "../Features/Misc/AutoDialog.h"
#include "../Features/Misc/LootManager.h"
#include "../Features/Misc/OpenPose.h"
#include "../Features/Misc/AutoDestroy.h"
#include "../Features/Misc/ObjectUnlocker.h"
#include "../Features/Misc/UnlockStatues.h"
#include "../Features/Misc/OpenChest.h"
#include "../Features/Misc/LootMagnet.h"
#include "../Features/Misc/SpawnSomething.h"
#include "../Features/Misc/Uncensored.h"
#include "../Features/Misc/FpsUnlock.h"
#include "../Features/Misc/BoobaScale.h"
#include "../Features/Misc/KeepCamera.h"
#include "../Features/Misc/SkipCutscenes.h"
#include "../Features/Misc/CustomElement.h"
#include "../Features/Misc/MapSettings.h"
#include "../Features/Profiles/DefaultProfile.h"
#include "../Features/Combat/InstantBowFeature.h"
#include "../Features/Move/BetteFly.h"
#include "../Features/Visual/CustomEffect.h"
#include "../Features/Combat/CombatProp.h"
#include "../Features/Misc/PenisDraw.h"
#include "../Features/Visual/Trail.h"
#include "../Features/Fun/coolMoon.h"
#include "../Features/Fun/Minecraft.h"
#include "../Offsets.h"
#include "../Utils/Embedded/zh_cn_ttf_blob.h"
#include "../Utils/Embedded/radius_texture_blob.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
 ID3D11ShaderResourceView* g_radiusSrv = nullptr;
 ComPtr<ID3D11Device> g_device{ nullptr };
static int g_radiusTexW = 0;
static int g_radiusTexH = 0;
extern ImFont* g_fontRegular;
extern ImFont* g_fontBold;
extern ImFont* g_fontNameTags;
extern bool g_showSettings;



#define DECLARE_GAME_HOOK(offset, ret, name, ...) \
    extern ret (*name##_Orig)(__VA_ARGS__); \
    extern ret name##_Hook(__VA_ARGS__);

#define GAME_HOOK(offset, ret, name, ...) DECLARE_GAME_HOOK(offset, ret, name, __VA_ARGS__)
#include "GameHooks.def"
#undef GAME_HOOK
#undef DECLARE_GAME_HOOK

ImFont* mainFont = nullptr;
ImFont* tabFont = nullptr;

namespace
{
    App* g_globalApp = nullptr;
    std::atomic<int> g_mouseWheelSteps{ 0 };
}

void SetGlobalAppInstance(App* app)
{
    g_globalApp = app;
}

void TickGlobalAppFromGameUpdate()
{
    if (g_globalApp)
        g_globalApp->Run();
}

int ConsumeGlobalMouseWheelSteps()
{
    return g_mouseWheelSteps.exchange(0, std::memory_order_acq_rel);
}

namespace
{
    std::vector<void*> g_apiHooks;
    std::vector<void*> g_gameHooks;

    
    ComPtr<ID3D11DeviceContext> g_context;
    ComPtr<ID3D11RenderTargetView> g_rtv;

    bool g_shuttingDown{ false };
    bool g_imguiInit{ false };


    HWND g_hwnd = nullptr;
    WNDPROC g_oldWndProc = nullptr;

    SHORT(WINAPI* g_GetAsyncKeyStateOrig)(int vKey) { nullptr };

    bool IsFeatureBindKey(int vKey)
    {
        if (vKey <= 0)
            return false;
        auto& features = GetFeatureManager().All();
        for (const auto& entry : features)
        {
            if (!entry)
                continue;
            if (entry->KeybindKey() == vKey)
                return true;
        }
        return false;
    }

    bool IsGameWindowActive()
    {
        if (!g_hwnd)
            return false;

        HWND foreground = LI(user32, GetForegroundWindow)();
        return foreground == g_hwnd || LI(user32, IsChild)(g_hwnd, foreground);
    }

    static void InitImGui(HWND hwnd);
    void ShutdownImGui()
    {
        if (!g_imguiInit)
            return;

        if (g_hwnd && g_oldWndProc) {
            LI(user32, SetWindowLongPtrA)(g_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_oldWndProc));
            g_oldWndProc = nullptr;
        }

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        g_imguiInit = false;
    }
    HRESULT(__stdcall* oPresent11)(IDXGISwapChain* swapChain, UINT sync, UINT flags);
    HRESULT __stdcall hkPresent11(IDXGISwapChain* swapChain, UINT sync, UINT flags)
    {
        if (!swapChain || !oPresent11)
            return E_FAIL;

        if (g_shuttingDown)
            return oPresent11(swapChain, sync, flags);

        if (!g_device) {
            ID3D11Device* device = nullptr;
            if (SUCCEEDED(swapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&device)))) {
                g_device.Attach(device);
                ID3D11DeviceContext* context = nullptr;
                g_device->GetImmediateContext(&context);
                g_context.Attach(context);
            }
        }

        if (g_device && !g_rtv) {
            ID3D11Texture2D* backBuffer = nullptr;
            if (SUCCEEDED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)))) {
                ID3D11RenderTargetView* rtv = nullptr;
                if (SUCCEEDED(g_device->CreateRenderTargetView(backBuffer, nullptr, &rtv)))
                    g_rtv.Attach(rtv);
                backBuffer->Release();
            }
        }

        if (g_device && g_context && g_rtv && !g_imguiInit) {
            DXGI_SWAP_CHAIN_DESC desc{};
            if (SUCCEEDED(swapChain->GetDesc(&desc))) {
                g_hwnd = desc.OutputWindow;
                InitImGui(g_hwnd);
            }
        }

        if (g_imguiInit) {
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            if (g_loginDoorOverlayActive) {
                ImGuiIO& io = ImGui::GetIO();
                ImGui::GetForegroundDrawList()->AddRectFilled(
                    ImVec2(0.0f, 0.0f),
                    io.DisplaySize,
                    IM_COL32(0, 0, 0, 255));
            }

            GetFeatureManager().ImGuiDrawAll();
            GuiRender(); // тут жоско *****

            ImGui::Render();
            ID3D11RenderTargetView* rtv = g_rtv.Get();
            g_context->OMSetRenderTargets(1, &rtv, nullptr);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }

        return oPresent11(swapChain, sync, flags);
    }

    SHORT WINAPI GetAsyncKeyState_hook(int vKey)
    {
        if (IsFeatureBindKey(vKey))
            return g_GetAsyncKeyStateOrig ? g_GetAsyncKeyStateOrig(vKey) : 0;

        if (!IsGameWindowActive())
            return 0;

        if (vKey == VK_RSHIFT || vKey == VK_END) return g_GetAsyncKeyStateOrig ? g_GetAsyncKeyStateOrig(vKey) : 0;

        if (g_showSettings && g_imguiInit)
        {
            ImGuiIO& io = ImGui::GetIO();
            if (io.WantCaptureKeyboard) return 0;
        }

        return g_GetAsyncKeyStateOrig ? g_GetAsyncKeyStateOrig(vKey) : 0;
    }

    void InitHooks()
    {
        #define INIT_GAME_HOOK(offset, ret, name, ...) \
            HookGameFunction(offset, reinterpret_cast<LPVOID>(&::name##_Hook), reinterpret_cast<void**>(&::name##_Orig), g_gameHooks);

    #define GAME_HOOK(offset, ret, name, ...) INIT_GAME_HOOK(offset, ret, name, __VA_ARGS__)
    #include "GameHooks.def"
#include "../Utils/test01.h"






    #undef GAME_HOOK
    #undef INIT_GAME_HOOK

    //telemetry_filter::Install(g_gameHooks);

    HookApiFunction(XS("user32.dll"), XS("GetAsyncKeyState"), &GetAsyncKeyState_hook, reinterpret_cast<void**>(&g_GetAsyncKeyStateOrig), g_apiHooks);
}



    LRESULT CALLBACK WndProcHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (!IsGameWindowActive())
            return g_oldWndProc ? CallWindowProc(g_oldWndProc, hwnd, msg, wParam, lParam) : DefWindowProc(hwnd, msg, wParam, lParam);

        if (msg == WM_MOUSEWHEEL && !g_showSettings)
        {
            int steps = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            if (steps == 0)
                steps = (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ? 1 : -1;
            g_mouseWheelSteps.fetch_add(steps, std::memory_order_acq_rel);
        }

        if (g_imguiInit){ //&& g_showSettings) {
            ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
            ImGuiIO& io = ImGui::GetIO();

            if ((msg >= WM_KEYFIRST && msg <= WM_KEYLAST) || msg == WM_CHAR || msg == WM_SYSCHAR)  if (io.WantCaptureKeyboard) return 1;
            
            if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST)  if (io.WantCaptureMouse) return 1;
            
        }
        return g_oldWndProc ? CallWindowProc(g_oldWndProc, hwnd, msg, wParam, lParam) : DefWindowProc(hwnd, msg, wParam, lParam);
    }

    static void InitRadiusTexture()
    {
        if (g_radiusSrv) return;
        dxUtils::LoadTextureFromMemory(
            embedded_radius_texture::data,
            embedded_radius_texture::size,
            &g_radiusSrv,
            &g_radiusTexW,
            &g_radiusTexH
        );
    }

    static void InitImGui(HWND hwnd)
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

        
        char windowsDir[MAX_PATH]{};
        if (LI(kernel32, GetWindowsDirectoryA)(windowsDir, MAX_PATH))
        {
            std::string fontPath = std::string(windowsDir) + XS("\\Fonts\\segoeui.ttf");
            std::string boldPath = std::string(windowsDir) + XS("\\Fonts\\segoeuib.ttf");
            ImFontGlyphRangesBuilder builder;
            builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
            builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
            ImVector<ImWchar> ranges;
            builder.BuildRanges(&ranges);
            mainFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 19.0f, nullptr, ranges.Data);
            tabFont = io.Fonts->AddFontFromFileTTF(boldPath.c_str(), 20.0f, nullptr, ranges.Data);

            ImFontGlyphRangesBuilder nameBuilder;
            nameBuilder.AddRanges(io.Fonts->GetGlyphRangesDefault());
            nameBuilder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
            nameBuilder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
            ImVector<ImWchar> nameRanges;
            nameBuilder.BuildRanges(&nameRanges);
            ImFontConfig nameCfg{};
            nameCfg.FontDataOwnedByAtlas = false;
            g_fontNameTags = io.Fonts->AddFontFromMemoryTTF(
                const_cast<unsigned char*>(embedded_zh_cn_ttf::data),
                static_cast<int>(embedded_zh_cn_ttf::size),
                18.0f,
                &nameCfg,
                nameRanges.Data
            );
        }
        if (!mainFont)
            mainFont = io.Fonts->AddFontDefault();
        if (!tabFont)
            tabFont = mainFont;
        if (!g_fontNameTags)
            g_fontNameTags = mainFont;

        g_fontRegular = mainFont;
        g_fontBold = tabFont;
        io.FontDefault = g_fontRegular;

        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(g_device.Get(), g_context.Get());

        InitRadiusTexture();

        g_oldWndProc = reinterpret_cast<WNDPROC>(LI(user32, SetWindowLongPtrA)(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook)));
        g_imguiInit = true;

        GuiInit(io);
    }
}


void App::InitFeatures()
{
    auto& features = GetFeatureManager();
    features.AddFeature<GodModeFeature>();
    features.AddFeature<InfiniteSkillsFeature>();
    features.AddFeature<VoidAuraFeature>();
    features.AddFeature<AttackMultiplierFeature>();
    features.AddFeature<FriendlyFireFeature>();
    features.AddFeature<InstantBowFeature>();
	features.AddFeature<KillAuraFeature>();
    features.AddFeature<CombatPropFeature>();

    features.AddFeature<VelocityFeature>();
	features.AddFeature<BetterFlyFeature>();
    features.AddFeature<InfiniteStaminaFeature>();
    features.AddFeature<SprintDelayFeature>();
    features.AddFeature<NoClipFeature>();
    features.AddFeature<JesusFeature>();
    features.AddFeature<MapTpFeature>();
    features.AddFeature<CustomTpFeature>();
    features.AddFeature<AirJumpFeature>();
    features.AddFeature<SpinFeature>();
    features.AddFeature<BowTpFeature>();

    features.AddFeature<EspFeature>();
    features.AddFeature<SkeletonEspFeature>();
    features.AddFeature<JumpCircleFeature>();
    features.AddFeature<CustomEffectFeature>();
    features.AddFeature<ChamsFeature>();
    features.AddFeature<WireframeFeature>();
    features.AddFeature<GuiStyleFeature>();
    features.AddFeature<ArrayListFeature>();
    features.AddFeature<FpsOverlayFeature>();
    features.AddFeature<AtmosphereFeature>();
    features.AddFeature<CameraSettingsFeature>();
    features.AddFeature<NameTagsFeature>();
    features.AddFeature<TextureChangerFeature>();
    features.AddFeature<ProfileChangerFeature>();
    features.AddFeature<SkinChangerFeature>();
    features.AddFeature<TrailFeature>();

    features.AddFeature<AutoDialogFeature>();
    features.AddFeature<LootManagerFeature>();
    features.AddFeature<OpenPoseFeature>();
    features.AddFeature<AutoDestroyFeature>();
    features.AddFeature<ObjectUnlockerFeature>();
    //features.AddFeature<UnlockStatuesFeature>();
    features.AddFeature<OpenChestFeature>();
    features.AddFeature<LootMagnetFeature>();
    features.AddFeature<SpawnSomethingFeature>();
    features.AddFeature<UncensoredFeature>();
    features.AddFeature<FpsUnlockFeature>();
    features.AddFeature<BoobaScaleFeature>();
    //features.AddFeature<KeepCameraFeature>();
    features.AddFeature<SkipCutscenesFeature>();
    features.AddFeature<CustomElementFeature>();
    features.AddFeature<MapSettingsFeature>();
    features.AddFeature<DefaultProfileFeature>();
    features.AddFeature<PenisDrawFeature>();

    features.AddFeature<CoolMoonFeature>();
    features.AddFeature<MinecraftFeature>();

}

void App::Init()
{
    AllocConsole();
    Logger::console = GetStdHandle(STD_OUTPUT_HANDLE);

    HANDLE hNul = LI(kernel32, CreateFileA)(
        XS("NUL"),
        GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (hNul && hNul != INVALID_HANDLE_VALUE)
    {
        LI(kernel32, SetStdHandle)(STD_OUTPUT_HANDLE, hNul);
        LI(kernel32, SetStdHandle)(STD_ERROR_HANDLE, hNul);
    }

    FILE* fOut = nullptr;
    FILE* fErr = nullptr;
    freopen_s(&fOut, "NUL", "w", stdout);
    freopen_s(&fErr, "NUL", "w", stderr);

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::setlocale(LC_ALL, ".UTF8");
    SetConsoleTextAttribute(Logger::console, 0x000F);

    InitFeatures();
}

App::App(HMODULE module)
    : module_(module)
   // , features_()
{
}

void App::Bootstrap()
{
    bool expected = false;
    if (!bootstrapped_.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
        return;

    g_GameBase = reinterpret_cast<uintptr_t>(LI(kernel32, GetModuleHandleA)(nullptr));
    const MH_STATUS mh = MH_Initialize();
    if (mh != MH_OK && mh != MH_ERROR_ALREADY_INITIALIZED)
    {
        bootstrapped_.store(false, std::memory_order_release);
        return;
    }

    InitHooks();
}

void App::TryInitRenderHook()
{
    if (renderHookBound_.load(std::memory_order_acquire))
        return;

    if (kiero::init(kiero::RenderType::D3D11) != kiero::Status::Success)
        return;

    kiero::bind(8, reinterpret_cast<void**>(&oPresent11), hkPresent11);
    renderHookBound_.store(true, std::memory_order_release);
}

void App::Run()
{
    if (!bootstrapped_.load(std::memory_order_acquire))
        Bootstrap();

    std::call_once(initOnce_, [this]()
    {
        Init();
    });

    if (!running_.load(std::memory_order_acquire))
    {
        if (!shutdownDone_.exchange(true, std::memory_order_acq_rel))
            Shutdown();
        return;
    }

    TryInitRenderHook();
    Update();
}

void App::Shutdown()
{
    g_shuttingDown = true;
    g_showSettings = false;

    GetFeatureManager().ShutdownAll();
    ShutdownImGui();
    for (void* h : g_gameHooks)
        MH_DisableHook(h);
    g_gameHooks.clear();

    for (void* h : g_apiHooks)
        MH_DisableHook(h);
    g_apiHooks.clear();

    if (renderHookBound_.exchange(false, std::memory_order_acq_rel))
        kiero::shutdown();

    if (bootstrapped_.exchange(false, std::memory_order_acq_rel))
        MH_Uninitialize();

    g_rtv.Reset();
    g_context.Reset();
    g_device.Reset();
}

void App::Update()
{
    SHORT(*keyFn)(int) = g_GetAsyncKeyStateOrig ? g_GetAsyncKeyStateOrig : GetAsyncKeyState;
    static bool prevEnd = false;
    static bool prevShift = false;

    const bool gameActive = IsGameWindowActive();
    bool endDown = gameActive && ((keyFn(VK_END) & 0x8000) != 0);
    if (endDown && !prevEnd)
    {
        g_showSettings = false;
        g_shuttingDown = true;
        running_.store(false);
    }
    prevEnd = endDown;

    bool shiftDown = gameActive && ((keyFn(VK_RSHIFT) & 0x8000) != 0);
    if (shiftDown && !prevShift)
        g_showSettings = !g_showSettings;
    prevShift = shiftDown;
    
    GetFeatureManager().UpdateAll();
}
