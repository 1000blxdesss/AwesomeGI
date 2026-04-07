
#include "gui.h"
#include "imgui_internal.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <deque>
#include "../globals.h"
#include "../Features/FeatureBase.h"
#include "../Features/Profiles/DefaultProfile.h"
#include "../Utils/test02.h"

ImFont* g_fontRegular = nullptr;
ImFont* g_fontBold = nullptr;
ImFont* g_fontNameTags = nullptr;
bool g_showSettings = false;


extern ImFont* mainFont;
extern ImFont* tabFont;

namespace
{
    bool g_guiStyleApplied = false;
    float g_menuScroll = 0.f;
    float g_waypointDockAnim = 0.f;

    const ImVec4 kDefaultGuiColorWindow = ImVec4(0.005f, 0.005f, 0.006f, 0.97f);
    const ImVec4 kDefaultGuiColorChild = ImVec4(0.007f, 0.007f, 0.008f, 0.98f);
    const ImVec4 kDefaultGuiColorPopup = ImVec4(0.010f, 0.010f, 0.012f, 0.99f);
    const ImVec4 kDefaultGuiColorFrame = ImVec4(0.016f, 0.016f, 0.020f, 1.0f);
    const ImVec4 kDefaultGuiColorButton = ImVec4(0.022f, 0.022f, 0.028f, 1.0f);
    const ImVec4 kDefaultGuiColorText = ImVec4(0.94f, 0.94f, 0.98f, 1.0f);
    const ImVec4 kDefaultGuiColorTextEnabled = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    const ImVec4 kDefaultGuiColorTextDisabled = ImVec4(0.2353f, 0.2353f, 0.2588f, 0.6510f);
    const ImVec4 kDefaultGuiOverlayColor = ImVec4(0.f, 0.f, 0.f, 0.90f);
    const ImVec4 kDefaultGuiColorAccent = ImVec4(0.39607844f, 0.39607844f, 0.39607844f, 1.0f);//ImVec4(0.72f, 0.18f, 0.56f, 1.0f);
    const ImVec4 kDefaultGuiColorOutline = ImVec4(0.0156862745f, 0.0156862745f, 0.0156862745f, 245.f / 255.f);
    const ImVec4 kDefaultGuiColorHighlight = ImVec4(1.0f, 1.0f, 1.0f, 0.01176471f);//ImVec4(214.f / 255.f, 0.f, 151.f / 255.f, 3.f / 255.f);
    const ImVec4 kDefaultGuiColorBadge = ImVec4(0.10f, 0.10f, 0.14f, 0.80f);

    ImVec4 g_guiColorWindow = kDefaultGuiColorWindow;
    ImVec4 g_guiColorChild = kDefaultGuiColorChild;
    ImVec4 g_guiColorPopup = kDefaultGuiColorPopup;
    ImVec4 g_guiColorFrame = kDefaultGuiColorFrame;
    ImVec4 g_guiColorButton = kDefaultGuiColorButton;
    ImVec4 g_guiColorText = kDefaultGuiColorText;
    ImVec4 g_guiColorTextEnabled = kDefaultGuiColorTextEnabled;
    ImVec4 g_guiColorTextDisabled = kDefaultGuiColorTextDisabled;
    ImVec4 g_guiOverlayColor = kDefaultGuiOverlayColor;
    ImVec4 g_guiColorAccent = kDefaultGuiColorAccent;
    ImVec4 g_guiColorOutline = kDefaultGuiColorOutline;
    ImVec4 g_guiColorHighlight = kDefaultGuiColorHighlight;
    ImVec4 g_guiColorBadge = kDefaultGuiColorBadge;
    ImVec4 g_activeAccent = kDefaultGuiColorAccent;

    

    

    static void ApplyGuiStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 12.f;
        style.ChildRounding = 10.f;
        style.FrameRounding = 8.f;
        style.ScrollbarRounding = 12.f;
        style.GrabRounding = 6.f;
        style.WindowBorderSize = 0.f;
        style.FrameBorderSize = 0.f;
        style.IndentSpacing = 18.f;

        style.Colors[ImGuiCol_WindowBg] = g_guiColorWindow;
        style.Colors[ImGuiCol_ChildBg] = g_guiColorChild;
        style.Colors[ImGuiCol_PopupBg] = g_guiColorPopup;
        style.Colors[ImGuiCol_FrameBg] = g_guiColorFrame;
        style.Colors[ImGuiCol_FrameBgHovered] = ImLerp(g_guiColorFrame, ImVec4(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 1.0f), 0.28f);
        style.Colors[ImGuiCol_FrameBgActive] = ImLerp(g_guiColorFrame, ImVec4(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 1.0f), 0.52f);
        style.Colors[ImGuiCol_Button] = g_guiColorButton;
        style.Colors[ImGuiCol_ButtonHovered] = ImLerp(g_guiColorButton, ImVec4(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 1.0f), 0.38f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, ImClamp(g_guiColorAccent.w, 0.f, 1.f));
        style.Colors[ImGuiCol_Text] = g_guiColorText;
        style.Colors[ImGuiCol_TextDisabled] = g_guiColorTextDisabled;
        style.Colors[ImGuiCol_Border] = g_guiColorOutline;

        g_activeAccent = g_guiColorAccent;
    }

    static void ResetGuiColorsInternal()
    {
        g_guiColorWindow = kDefaultGuiColorWindow;
        g_guiColorChild = kDefaultGuiColorChild;
        g_guiColorPopup = kDefaultGuiColorPopup;
        g_guiColorFrame = kDefaultGuiColorFrame;
        g_guiColorButton = kDefaultGuiColorButton;
        g_guiColorText = kDefaultGuiColorText;
        g_guiColorTextEnabled = kDefaultGuiColorTextEnabled;
        g_guiColorTextDisabled = kDefaultGuiColorTextDisabled;
        g_guiOverlayColor = kDefaultGuiOverlayColor;
        g_guiColorAccent = kDefaultGuiColorAccent;
        g_guiColorOutline = kDefaultGuiColorOutline;
        g_guiColorHighlight = kDefaultGuiColorHighlight;
        g_guiColorBadge = kDefaultGuiColorBadge;
        g_activeAccent = g_guiColorAccent;
    }
    static bool CapsuleTab(const char* label, bool selected, float width)
    {

        ImFont* font = tabFont ? tabFont : ImGui::GetFont();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float height = ImGui::GetFontSize() + 18.f;
        ImVec2 size(width, height);

        ImGui::SetCursorScreenPos(pos);
        bool pressed = ImGui::InvisibleButton(label, size);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        bool hovered = ImGui::IsItemHovered();
        bool active = selected || ImGui::IsItemActive();

        ImVec4 accent = ImVec4(g_activeAccent.x, g_activeAccent.y, g_activeAccent.z, 0.98f);
        ImVec4 hover = ImVec4(0.24f, 0.24f, 0.28f, 0.82f);

        ImRect rect(pos, ImVec2(pos.x + width, pos.y + height));
        const float rounding = 28.f;

        if (active)
            dl->AddRectFilled(rect.Min, rect.Max, ImGui::ColorConvertFloat4ToU32(accent), rounding);
        else if (hovered)
            dl->AddRectFilled(rect.Min, rect.Max, ImGui::ColorConvertFloat4ToU32(hover), rounding);

        ImVec2 textSize = font->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.f, label);
        ImVec2 textPos(rect.Min.x + (rect.GetWidth() - textSize.x) * 0.5f,
            rect.Min.y + (rect.GetHeight() - textSize.y) * 0.5f - 1.f);

        ImGui::PushFont(font);
        dl->AddText(textPos, IM_COL32(255, 255, 255, 255), label);
        ImGui::PopFont();

        return pressed;
    }
    static ImVec4 AccentForTab(int)
    {
        return g_activeAccent;
    }
    struct DropdownAnimState
    {
        bool open = false;
        float anim = 0.f;
    };
    static std::unordered_map<ImGuiID, DropdownAnimState> g_dropdownStates;
    static std::unordered_map<std::string, CardState> g_profileCfgCards;
    static std::string g_selectedProfileCfg;

    struct HeaderSplitConfig
    {
        int* value = nullptr;
        bool* linkedBool = nullptr;
        const char* leftLabel = "<";
        const char* rightLabel = ">";
    };
    static std::unordered_map<ImGuiID, HeaderSplitConfig> g_headerSplitConfigs;

    struct PendingSplitBadge
    {
        const char* cardId = nullptr;
        int* value = nullptr;
        bool* linkedBool = nullptr;
        const char* leftLabel = nullptr;
        const char* rightLabel = nullptr;
    };
    static PendingSplitBadge g_pendingSplitBadge;
    static Feature* g_currentGuiFeature = nullptr;

    static bool IsLikelyStackPointer(const void* ptr)
    {
        if (!ptr)
            return false;
        const uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        const char marker = 0;
        const uintptr_t sp = reinterpret_cast<uintptr_t>(&marker);
        const uintptr_t span = 8ull * 1024ull * 1024ull;
        return addr >= (sp - span) && addr <= (sp + span);
    }

    static void BindBoolFieldIfSafe(const char* id, bool* value)
    {
        if (!g_currentGuiFeature || !id || !id[0] || !value || IsLikelyStackPointer(value))
            return;
        g_currentGuiFeature->BindConfigBool(id, value);
    }

    static void BindIntFieldIfSafe(const char* id, int* value)
    {
        if (!g_currentGuiFeature || !id || !id[0] || !value || IsLikelyStackPointer(value))
            return;
        g_currentGuiFeature->BindConfigInt(id, value);
    }

    static void BindFloatFieldIfSafe(const char* id, float* value)
    {
        if (!g_currentGuiFeature || !id || !id[0] || !value || IsLikelyStackPointer(value))
            return;
        g_currentGuiFeature->BindConfigFloat(id, value);
    }

    static void BindStringFieldIfSafe(const char* id, char* value, size_t size)
    {
        if (!g_currentGuiFeature || !id || !id[0] || !value || size == 0 || IsLikelyStackPointer(value))
            return;
        g_currentGuiFeature->BindConfigString(id, value, size);
    }

    static void BindColorFieldIfSafe(const char* id, ImVec4* value)
    {
        if (!g_currentGuiFeature || !id || !id[0] || !value || IsLikelyStackPointer(value))
            return;
        g_currentGuiFeature->BindConfigColor(id, value);
    }

    static std::string ToUpperAscii(std::string value)
    {
        for (char& ch : value)
            ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        return value;
    }

    static bool IsNoneBadgeText(const char* badge)
    {
        if (!badge || !badge[0])
            return false;
        if (std::strcmp(badge, XS("NONE")) == 0 || std::strcmp(badge, "NONE") == 0)
            return true;
        return ToUpperAscii(std::string(badge)) == "NONE";
    }

    static bool IsBindHoldRow(const char* label, const char* valueText)
    {
        if (!label || !valueText)
            return false;
        const bool labelMatch = (std::strcmp(label, XS("Key")) == 0) || (std::strcmp(label, "Key") == 0);
        const bool valueMatch = (std::strcmp(valueText, XS("Hold")) == 0) || (std::strcmp(valueText, "Hold") == 0);
        return labelMatch && valueMatch;
    }

    static std::string VirtualKeyToDisplayName(int vk)
    {
        switch (vk)
        {
        case VK_BACK: return "BACKSPACE";
        case VK_TAB: return "TAB";
        case VK_RETURN: return "ENTER";
        case VK_ESCAPE: return "ESC";
        case VK_SPACE: return "SPACE";
        case VK_SHIFT: return "SHIFT";
        case VK_CONTROL: return "CTRL";
        case VK_MENU: return "ALT";
        case VK_LSHIFT: return "LSHIFT";
        case VK_RSHIFT: return "RSHIFT";
        case VK_LCONTROL: return "LCTRL";
        case VK_RCONTROL: return "RCTRL";
        case VK_LMENU: return "LALT";
        case VK_RMENU: return "RALT";
        case VK_INSERT: return "INS";
        case VK_DELETE: return "DEL";
        case VK_HOME: return "HOME";
        case VK_END: return "END";
        case VK_PRIOR: return "PGUP";
        case VK_NEXT: return "PGDN";
        case VK_LEFT: return "LEFT";
        case VK_RIGHT: return "RIGHT";
        case VK_UP: return "UP";
        case VK_DOWN: return "DOWN";
        }

        if (vk >= 'A' && vk <= 'Z')
            return std::string(1, static_cast<char>(vk));
        if (vk >= '0' && vk <= '9')
            return std::string(1, static_cast<char>(vk));
        if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9)
            return std::string("NUM") + static_cast<char>('0' + (vk - VK_NUMPAD0));
        if (vk >= VK_F1 && vk <= VK_F24)
            return std::string("F") + std::to_string(vk - VK_F1 + 1);

        UINT scanCode = MapVirtualKeyA(static_cast<UINT>(vk), MAPVK_VK_TO_VSC);
        if (vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN ||
            vk == VK_PRIOR || vk == VK_NEXT || vk == VK_HOME || vk == VK_END ||
            vk == VK_INSERT || vk == VK_DELETE)
        {
            scanCode |= 0xE000;
        }

        char keyName[64] = {};
        const LONG lParam = static_cast<LONG>(scanCode << 16);
        const int copied = GetKeyNameTextA(lParam, keyName, static_cast<int>(sizeof(keyName)));
        if (copied > 0)
            return ToUpperAscii(std::string(keyName, static_cast<size_t>(copied)));

        return std::string("VK_") + std::to_string(vk);
    }

    static std::string BuildFeatureBadgeText(const Feature& feature, const char* fallbackBadge)
    {
        if (feature.KeybindCapturing())
            return "...";
        if (feature.KeybindKey() != 0)
            return VirtualKeyToDisplayName(feature.KeybindKey());
        if (IsNoneBadgeText(fallbackBadge))
            return "NONE";
        if (fallbackBadge && fallbackBadge[0])
            return fallbackBadge;
        return {};
    }

    static bool ShouldUseFeatureKeybindBadge(const Feature& feature, const char* fallbackBadge)
    {
        return feature.KeybindCapturing() || feature.KeybindKey() != 0 || IsNoneBadgeText(fallbackBadge);
    }

    static int PollPressedVirtualKeyForCapture()
    {
        auto pressed = [](ImGuiKey key) -> bool
            {
                return ImGui::IsKeyPressed(key, false);
            };

        if (pressed(ImGuiKey_Backspace)) return VK_BACK;
        if (pressed(ImGuiKey_Escape)) return VK_ESCAPE;
        if (pressed(ImGuiKey_Tab)) return VK_TAB;
        if (pressed(ImGuiKey_Enter) || pressed(ImGuiKey_KeypadEnter)) return VK_RETURN;
        if (pressed(ImGuiKey_Space)) return VK_SPACE;
        if (pressed(ImGuiKey_LeftShift)) return VK_LSHIFT;
        if (pressed(ImGuiKey_RightShift)) return VK_RSHIFT;
        if (pressed(ImGuiKey_LeftCtrl)) return VK_LCONTROL;
        if (pressed(ImGuiKey_RightCtrl)) return VK_RCONTROL;
        if (pressed(ImGuiKey_LeftAlt)) return VK_LMENU;
        if (pressed(ImGuiKey_RightAlt)) return VK_RMENU;
        if (pressed(ImGuiKey_Insert)) return VK_INSERT;
        if (pressed(ImGuiKey_Delete)) return VK_DELETE;
        if (pressed(ImGuiKey_Home)) return VK_HOME;
        if (pressed(ImGuiKey_End)) return VK_END;
        if (pressed(ImGuiKey_PageUp)) return VK_PRIOR;
        if (pressed(ImGuiKey_PageDown)) return VK_NEXT;
        if (pressed(ImGuiKey_LeftArrow)) return VK_LEFT;
        if (pressed(ImGuiKey_RightArrow)) return VK_RIGHT;
        if (pressed(ImGuiKey_UpArrow)) return VK_UP;
        if (pressed(ImGuiKey_DownArrow)) return VK_DOWN;

        for (int i = 0; i < 26; ++i)
        {
            if (pressed(static_cast<ImGuiKey>(ImGuiKey_A + i)))
                return 'A' + i;
        }
        for (int i = 0; i < 10; ++i)
        {
            if (pressed(static_cast<ImGuiKey>(ImGuiKey_0 + i)))
                return '0' + i;
        }
        for (int i = 0; i < 12; ++i)
        {
            if (pressed(static_cast<ImGuiKey>(ImGuiKey_F1 + i)))
                return VK_F1 + i;
        }
        for (int i = 0; i < 10; ++i)
        {
            if (pressed(static_cast<ImGuiKey>(ImGuiKey_Keypad0 + i)))
                return VK_NUMPAD0 + i;
        }

        if (pressed(ImGuiKey_Minus)) return VK_OEM_MINUS;
        if (pressed(ImGuiKey_Equal)) return VK_OEM_PLUS;
        if (pressed(ImGuiKey_LeftBracket)) return VK_OEM_4;
        if (pressed(ImGuiKey_RightBracket)) return VK_OEM_6;
        if (pressed(ImGuiKey_Backslash)) return VK_OEM_5;
        if (pressed(ImGuiKey_Semicolon)) return VK_OEM_1;
        if (pressed(ImGuiKey_Apostrophe)) return VK_OEM_7;
        if (pressed(ImGuiKey_Comma)) return VK_OEM_COMMA;
        if (pressed(ImGuiKey_Period)) return VK_OEM_PERIOD;
        if (pressed(ImGuiKey_Slash)) return VK_OEM_2;
        if (pressed(ImGuiKey_GraveAccent)) return VK_OEM_3;
        if (pressed(ImGuiKey_CapsLock)) return VK_CAPITAL;
        if (pressed(ImGuiKey_ScrollLock)) return VK_SCROLL;
        if (pressed(ImGuiKey_NumLock)) return VK_NUMLOCK;
        if (pressed(ImGuiKey_Pause)) return VK_PAUSE;

        return 0;
    }

    static bool FunctionHeader(const char* id, const char* name, const char* desc, const char* badge, CardState& state, float cardWidth, float iconAnim,
        bool allowBadgeCapture = false, bool* outBadgeClicked = nullptr, ImVec2* outCardMin = nullptr, float* outHeaderH = nullptr)
    {
        if (outBadgeClicked)
            *outBadgeClicked = false;
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const float headerH = 60.f;

        ImVec2 cardMin = ImGui::GetCursorScreenPos();
        if (outCardMin) *outCardMin = cardMin;
        if (outHeaderH) *outHeaderH = headerH;

        ImVec2 cardMax(cardMin.x + cardWidth, cardMin.y + headerH);

        ImGui::SetCursorScreenPos(cardMin);
        ImGui::InvisibleButton(id, ImVec2(cardWidth, headerH));
        const bool headerLeftClicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
        const bool headerRightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);

        bool toggled = false;
        if (headerRightClicked)
        {
            state.expanded = !state.expanded;
            toggled = true;
        }
        bool consumeHeaderLeftClick = false;
        bool badgeClicked = false;

        bool hovered = ImGui::IsItemHovered();
        bool held = ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left);

        float dt = ImGui::GetIO().DeltaTime;
        float hoverTarget = hovered ? 1.f : 0.f;
        float pressTarget = (state.active || held) ? 1.f : 0.f;

        state.hover += (hoverTarget - state.hover) * ImClamp(dt * 18.f, 0.f, 1.f);
        state.pressGlow += (pressTarget - state.pressGlow) * ImClamp(dt * 20.f, 0.f, 1.f);

        float emphasis = ImSaturate(0.25f + state.hover * 0.5f + iconAnim * 0.35f + state.pressGlow * 0.25f);

        ImVec2 diamondCenter(cardMin.x + 28.f, cardMin.y + headerH * 0.5f);
        float d = 11.f + state.hover * 1.5f;
        float diamondAngle = IM_PI * 0.25f + iconAnim * (IM_PI * 0.50f);

        float accentAlpha = g_activeAccent.w > 0.f ? g_activeAccent.w : 0.88f;
        accentAlpha = ImClamp(accentAlpha, 0.35f, 1.0f);
        ImVec4 accent = ImVec4(g_activeAccent.x, g_activeAccent.y, g_activeAccent.z, accentAlpha);
        ImU32 accentFill = ImGui::ColorConvertFloat4ToU32(accent);
        ImVec2 pts[4];
        for (int i = 0; i < 4; ++i)
        {
            float baseAngle = (-IM_PI * 0.5f) + IM_PI * 0.5f * i;
            float a = baseAngle + diamondAngle;
            pts[i] = ImVec2(diamondCenter.x + cosf(a) * d, diamondCenter.y + sinf(a) * d);
        }
        dl->AddConvexPolyFilled(pts, 4, accentFill);

        auto drawLine = [&](ImVec2 a, ImVec2 b, float alpha)
            {
                if (alpha <= 0.001f) return;
                dl->AddLine(a, b, IM_COL32(255, 255, 255, (int)(alpha * 255.f)), 2.3f);
            };

        float lineLen = 6.f;
        float t = iconAnim;
        drawLine(add(diamondCenter,ImVec2(-lineLen, 0)), add(diamondCenter,ImVec2(lineLen, 0)), 1.0f - t);
        drawLine(add(diamondCenter,ImVec2(0, -lineLen)), add(diamondCenter,ImVec2(0, lineLen)), 1.0f - t);
        drawLine(add(diamondCenter,ImVec2(-5, -5)), add(diamondCenter,ImVec2(5, 5)), t);
        drawLine(add(diamondCenter,ImVec2(-5, 5)), add(diamondCenter,ImVec2(5, -5)), t);

        ImVec4 nameColor = ImVec4(1.f, 1.f, 1.f, 0.76f + 0.24f * emphasis);
        ImVec4 descColor = ImVec4(1.f, 1.f, 1.f, 0.24f + 0.32f * emphasis);

        ImGui::SetCursorScreenPos(ImVec2(cardMin.x + 58.f, cardMin.y + 12.f));
        ImGui::PushStyleColor(ImGuiCol_Text, nameColor);
        ImGui::TextUnformatted(name);
        ImGui::PopStyleColor();

        ImGui::SetCursorScreenPos(ImVec2(cardMin.x + 58.f, cardMin.y + 32.f));
        ImGui::PushStyleColor(ImGuiCol_Text, descColor);
        ImGui::TextUnformatted(desc);
        ImGui::PopStyleColor();

        ImGuiID headerId = window->GetID(id);
        auto splitIt = g_headerSplitConfigs.find(headerId);
        if (splitIt != g_headerSplitConfigs.end())
        {
            const HeaderSplitConfig& cfg = splitIt->second;
            const char* leftLabel = cfg.leftLabel ? cfg.leftLabel : "<";
            const char* rightLabel = cfg.rightLabel ? cfg.rightLabel : ">";

            ImVec2 leftSize = ImGui::CalcTextSize(leftLabel);
            ImVec2 rightSize = ImGui::CalcTextSize(rightLabel);
            float padX = 12.f;
            float padY = 4.f;
            float segmentWidth = ImMax(leftSize.x, rightSize.x) + padX * 2.f;
            float badgeHeight = ImMax(leftSize.y, rightSize.y) + padY * 2.f;
            ImVec2 badgeSize(segmentWidth * 2.f, badgeHeight);
            ImVec2 badgePos(cardMax.x - badgeSize.x - 18.f, cardMin.y + (headerH - badgeHeight) * 0.5f);

            ImRect leftRect(badgePos, ImVec2(badgePos.x + segmentWidth, badgePos.y + badgeHeight));
            ImRect rightRect(ImVec2(badgePos.x + segmentWidth, badgePos.y), add(badgePos,badgeSize));

            int current = (cfg.value && *cfg.value > 0) ? 1 : 0;

            ImGui::PushID("split_badge");
            ImGui::SetCursorScreenPos(leftRect.Min);
            ImGui::InvisibleButton("##left", leftRect.GetSize());
            bool leftHovered = ImGui::IsItemHovered();
            bool leftHeld = ImGui::IsItemActive();
            bool leftClicked = ImGui::IsItemClicked();
            if (leftClicked)
                current = 0;

            ImGui::SetCursorScreenPos(rightRect.Min);
            ImGui::InvisibleButton("##right", rightRect.GetSize());
            bool rightHovered = ImGui::IsItemHovered();
            bool rightHeld = ImGui::IsItemActive();
            bool rightClicked = ImGui::IsItemClicked();
            if (rightClicked)
                current = 1;
            if (leftClicked || rightClicked)
                consumeHeaderLeftClick = true;

            if (cfg.value)
                *cfg.value = current;
            if (cfg.linkedBool)
                *cfg.linkedBool = (current == 1);

            if (leftHovered || rightHovered) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

            ImVec4 fillColor = ImVec4(0.12f, 0.12f, 0.16f, 0.96f);
            ImVec4 borderColor = ImVec4(0.32f, 0.32f, 0.40f, 0.88f);

            dl->AddRectFilled(badgePos, add(badgePos,badgeSize),
                ImGui::ColorConvertFloat4ToU32(fillColor), badgeHeight * 0.5f);
            dl->AddRect(badgePos, add(badgePos,badgeSize),
                ImGui::ColorConvertFloat4ToU32(borderColor), badgeHeight * 0.5f, 0, 1.1f);

            dl->AddLine(ImVec2(badgePos.x + segmentWidth, badgePos.y + 6.f),
                ImVec2(badgePos.x + segmentWidth, badgePos.y + badgeHeight - 6.f),
                IM_COL32(80, 80, 92, 180), 1.1f);

            auto drawCentered = [&](const ImRect& rect, const char* text)
                {
                    ImVec2 size = ImGui::CalcTextSize(text);
                    ImVec2 pos(rect.Min.x + (rect.GetWidth() - size.x) * 0.5f,
                        rect.Min.y + (rect.GetHeight() - size.y) * 0.5f - 0.5f);
                    dl->AddText(pos, IM_COL32(235, 235, 245, 255), text);
                };

            drawCentered(leftRect, leftLabel);
            drawCentered(rightRect, rightLabel);
            ImGui::PopID();
        }
        else if (badge && badge[0])
        {
            ImVec2 textSize = ImGui::CalcTextSize(badge);
            ImVec2 badgeSize(textSize.x + 24.f, textSize.y + 8.f);
            ImVec2 badgePos(cardMax.x - badgeSize.x - 18.f, cardMin.y + (headerH - badgeSize.y) * 0.5f);
            if (allowBadgeCapture)
            {
                const ImVec2 badgeMax = add(badgePos, badgeSize);
                const bool badgeHovered = ImGui::IsMouseHoveringRect(badgePos, badgeMax, true);
                if (badgeHovered)
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

                badgeClicked = badgeHovered && headerLeftClicked;
                if (badgeClicked)
                    consumeHeaderLeftClick = true;
            }

            ImVec4 badgeFill = g_guiColorBadge;
            ImVec4 badgeBorder = ImLerp(badgeFill,
                ImVec4(g_guiColorOutline.x, g_guiColorOutline.y, g_guiColorOutline.z,
                    ImClamp(g_guiColorOutline.w + 0.2f, 0.f, 1.f)),
                0.45f);
            dl->AddRectFilled(badgePos, add(badgePos,badgeSize),
                ImGui::ColorConvertFloat4ToU32(badgeFill), badgeSize.y * 0.5f);
            dl->AddRect(badgePos, add(badgePos,badgeSize),
                ImGui::ColorConvertFloat4ToU32(badgeBorder), badgeSize.y * 0.5f);
            dl->AddText(add(badgePos,ImVec2(12.f, 3.f)), IM_COL32(225, 225, 235, 255), badge);
        }

        if (outBadgeClicked)
            *outBadgeClicked = badgeClicked;
        if (headerLeftClicked && !consumeHeaderLeftClick)
            state.active = !state.active;

        ImGui::SetCursorScreenPos(ImVec2(cardMin.x, cardMin.y + headerH));
        return toggled;
    }
    static float DetailRowText(const char* label, const char* value, const ImVec2& origin, float innerWidth, bool drawSeparator)
    {
        IM_UNUSED(drawSeparator);

        ImGui::SetCursorScreenPos(origin);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.60f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImVec2 valueSize = ImGui::CalcTextSize(value);
        ImGui::SetCursorScreenPos(ImVec2(origin.x + innerWidth - valueSize.x, origin.y));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.90f));
        ImGui::TextUnformatted(value);
        ImGui::PopStyleColor();

        return ImGui::GetFontSize() + 6.f;

        ImGui::SetCursorScreenPos(origin);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.60f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        valueSize = ImGui::CalcTextSize(value);
        ImGui::SetCursorScreenPos(ImVec2(origin.x + innerWidth - valueSize.x, origin.y));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.90f));
        ImGui::TextUnformatted(value);
        ImGui::PopStyleColor();

        return ImGui::GetFontSize() + 6.f;
    }
    static float ToggleTextRow(const char* id, const char* label, const char* valueText, bool& state,
        const ImVec2& origin, float innerWidth)
    {
        const char* shownText = valueText ? valueText : "";
        if (valueText && (strcmp(valueText, XS("Enabled")) == 0 || strcmp(valueText, XS("Disabled")) == 0))
            shownText = state ? XS("Enabled") : XS("Disabled");

        ImDrawList* dl = ImGui::GetWindowDrawList();
        float fontSize = ImGui::GetFontSize();
        const float rowHeight = fontSize + 8.f;

        float textBaseline = origin.y + (rowHeight - fontSize) * 0.5f - 0.5f;
        ImVec4 labelColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        labelColor.w *= 0.65f;
        dl->AddText(ImVec2(origin.x, textBaseline), ImGui::GetColorU32(labelColor), label);

        ImFont* font = ImGui::GetFont();
        float baseFontSize = ImGui::GetFontSize();
        ImVec2 valueSize = ImGui::CalcTextSize(shownText);
        float valueX = origin.x + innerWidth - valueSize.x;
        ImVec2 buttonPos(valueX - 4.f, origin.y);
        ImVec2 buttonSize(valueSize.x + 8.f, rowHeight);

        ImGui::PushID(id);
        ImGui::SetCursorScreenPos(buttonPos);
        ImGui::InvisibleButton("##value", buttonSize);
        bool hovered = ImGui::IsItemHovered();
        bool held = ImGui::IsItemActive();
        if (ImGui::IsItemClicked())
            state = !state;
        ImGui::PopID();

        if (hovered)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        ImVec4 valueColor = state ? GuiColorTextEnabled() : GuiColorTextDisabled();
        float scale = (hovered || held) ? 1.05f : 1.0f;
        fontSize = baseFontSize * scale;
        ImVec2 scaledSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.f, shownText);
        float drawX = origin.x + innerWidth - scaledSize.x;
        float drawY = origin.y + (rowHeight - fontSize) * 0.5f - 0.5f;
        dl->AddText(font, fontSize, ImVec2(drawX, drawY), ImGui::ColorConvertFloat4ToU32(valueColor), shownText);

        ImVec2 nextPos(origin.x, origin.y + rowHeight);
        ImGui::SetCursorScreenPos(nextPos);
        ImGui::Dummy(ImVec2(0.01f, 0.01f));
        ImGui::SetCursorScreenPos(nextPos);
        return rowHeight;
    }
    static float RangeSliderRow(const char* id, const char* label, float* v_min, float* v_max, float v_lower, float v_upper,
        const char* display_fmt, const ImVec2& origin, float innerWidth, bool brightLabel)
    {
        const float handleRadius = 6.f;
        const float trackHeight = 4.f;
        const float captionHeight = ImGui::GetFontSize();
        const float rowHeight = captionHeight + handleRadius * 2.f + 10.f;
        float sliderWidth = ImClamp(innerWidth * 0.55f, 140.f, innerWidth - 80.f);
        float sliderX = origin.x + innerWidth - sliderWidth;
        if (sliderX < origin.x + 90.f)
            sliderX = origin.x + 90.f;
        float sliderY = origin.y + captionHeight + 4.f;
        float labelY = sliderY + (handleRadius * 2.f - captionHeight) * 0.5f;

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec4 labelColor = brightLabel ? ImVec4(1.f, 1.f, 1.f, 0.92f) : ImVec4(1.f, 1.f, 1.f, 0.64f);
        ImGui::SetCursorScreenPos(ImVec2(origin.x, labelY));
        ImGui::PushStyleColor(ImGuiCol_Text, labelColor);
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImGui::PushID(id);

        const float range = (v_upper - v_lower) != 0.f ? (v_upper - v_lower) : 1.f;
        float tMin = ImClamp((*v_min - v_lower) / range, 0.f, 1.f);
        float tMax = ImClamp((*v_max - v_lower) / range, 0.f, 1.f);
        ImVec2 trackMin(sliderX, sliderY + handleRadius - trackHeight * 0.5f);
        ImVec2 trackMax(sliderX + sliderWidth, trackMin.y + trackHeight);
        ImVec2 handleMin(trackMin.x + sliderWidth * tMin, trackMin.y + trackHeight * 0.5f);
        ImVec2 handleMax(trackMin.x + sliderWidth * tMax, trackMin.y + trackHeight * 0.5f);

        ImGui::SetCursorScreenPos(ImVec2(sliderX, sliderY));
        ImGui::InvisibleButton("##range_slider", ImVec2(sliderWidth, handleRadius * 2.f));
        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();

        ImGuiStorage* storage = ImGui::GetStateStorage();
        ImGuiID activeKey = ImGui::GetID("range_active_handle");
        int activeHandle = storage->GetInt(activeKey, 0); // 0 = min, 1 = max

        if (ImGui::IsItemClicked())
        {
            float mouseT = ImClamp((ImGui::GetIO().MousePos.x - trackMin.x) / sliderWidth, 0.f, 1.f);
            activeHandle = (fabsf(mouseT - tMin) <= fabsf(mouseT - tMax)) ? 0 : 1;
            storage->SetInt(activeKey, activeHandle);
        }

        if (active && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            float mouseT = ImClamp((ImGui::GetIO().MousePos.x - trackMin.x) / sliderWidth, 0.f, 1.f);
            if (activeHandle == 0)
                tMin = ImMin(mouseT, tMax);
            else
                tMax = ImMax(mouseT, tMin);

            *v_min = v_lower + tMin * range;
            *v_max = v_lower + tMax * range;
        }

        const bool skyAccent = (g_activeAccent.z > g_activeAccent.x + 0.08f) &&
            (g_activeAccent.z > g_activeAccent.y + 0.04f);
        ImVec4 trackBase = ImLerp(g_guiColorFrame, ImVec4(0.f, 0.f, 0.f, g_guiColorFrame.w), 0.36f);
        ImVec4 trackBg = ImVec4(trackBase.x, trackBase.y, trackBase.z, 0.96f);
        float accentMix = skyAccent ? 0.16f : 0.28f;
        ImVec4 trackSel = ImVec4(1.f, 1.f, 1.f, 1.f);
        trackSel.w = skyAccent ? 0.82f : 0.90f;
        ImU32 bgCol = ImGui::ColorConvertFloat4ToU32(trackBg);
        ImU32 selCol = ImGui::ColorConvertFloat4ToU32(trackSel);
        dl->AddRectFilled(trackMin, trackMax, bgCol, trackHeight * 0.5f);
        dl->AddRectFilled(ImVec2(handleMin.x, trackMin.y), ImVec2(handleMax.x, trackMax.y), selCol, trackHeight * 0.5f);

        auto drawHandle = [&](const ImVec2& center, bool isLeft)
            {
                ImVec4 base = ImVec4(1.f, 1.f, 1.f, 0.95f);
                ImVec4 hover = ImVec4(1.f, 1.f, 1.f, 1.f);
                ImVec4 col = (hovered || active) ? hover : base;
                ImU32 fill = ImGui::ColorConvertFloat4ToU32(col);
                float d = handleRadius;
                dl->AddQuadFilled(
                    ImVec2(center.x, center.y - d),
                    ImVec2(center.x + d, center.y),
                    ImVec2(center.x, center.y + d),
                    ImVec2(center.x - d, center.y),
                    fill);
                float half = handleRadius * 0.45f;
                dl->AddLine(ImVec2(center.x - half, center.y), ImVec2(center.x + half, center.y), IM_COL32(255, 255, 255, 220), 1.6f);
                dl->AddLine(ImVec2(center.x, center.y - half), ImVec2(center.x, center.y + half), IM_COL32(255, 255, 255, 220), 1.6f);
                if (!isLeft)
                    dl->AddLine(ImVec2(center.x - half, center.y + half), ImVec2(center.x + half, center.y - half), IM_COL32(255, 255, 255, 220), 1.6f);
            };

        drawHandle(handleMin, true);
        drawHandle(handleMax, false);

        char minBuf[32]{};
        char maxBuf[32]{};
        _snprintf_s(minBuf, sizeof(minBuf), _TRUNCATE, display_fmt, *v_min);
        _snprintf_s(maxBuf, sizeof(maxBuf), _TRUNCATE, display_fmt, *v_max);

        ImVec2 minSize = ImGui::CalcTextSize(minBuf);
        ImVec2 maxSize = ImGui::CalcTextSize(maxBuf);

        ImVec2 minPos(sliderX + tMin * sliderWidth - minSize.x * 0.5f, sliderY - captionHeight - 2.f);
        ImVec2 maxPos(sliderX + tMax * sliderWidth - maxSize.x * 0.5f, sliderY - captionHeight - 2.f);

        ImVec4 textColor = ImVec4(1.f, 1.f, 1.f, 0.95f);
        ImU32 txtCol = ImGui::ColorConvertFloat4ToU32(textColor);
        dl->AddText(minPos, txtCol, minBuf);
        dl->AddText(maxPos, txtCol, maxBuf);

        ImGui::PopID();

        ImVec2 nextPos(origin.x, origin.y + rowHeight);
        ImGui::SetCursorScreenPos(nextPos);
        ImGui::Dummy(ImVec2(0.01f, 0.01f));
        ImGui::SetCursorScreenPos(nextPos);
        return rowHeight;
    }
    static float DualListInputRow(const char* id,
        const char* leftLabel, const char* rightLabel,
        char* leftBuffer, size_t leftBufferSize,
        char* rightBuffer, size_t rightBufferSize,
        bool& leftAddTriggered, bool& rightAddTriggered,
        const ImVec2& origin, float innerWidth)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImDrawListSplitter splitter;
        splitter.Split(dl, 2);
        splitter.SetCurrentChannel(dl, 1);

        leftAddTriggered = false;
        rightAddTriggered = false;

        const float fontSize = ImGui::GetFontSize();
        const float labelHeight = fontSize + 2.f;
        const float fieldHeight = fontSize + 12.f;
        const float rowHeight = labelHeight + fieldHeight + 12.f;
        const float separatorX = origin.x + innerWidth * 0.5f;
        const float badgeWidth = ImClamp(fontSize + 4.f, 16.f, 22.f);
        const float inputPad = 6.f;
        const float inputYOffset = (fieldHeight - fontSize) * 0.5f;

        ImVec2 capsuleMin(origin.x, origin.y + labelHeight + 6.f);
        ImVec2 capsuleMax(origin.x + innerWidth, capsuleMin.y + fieldHeight);

        ImVec2 leftBadgeMin = capsuleMin;
        ImVec2 leftBadgeMax(capsuleMin.x + badgeWidth, capsuleMax.y);
        ImVec2 rightBadgeMin(capsuleMax.x - badgeWidth, capsuleMin.y);
        ImVec2 rightBadgeMax = capsuleMax;

        float leftInputX = leftBadgeMax.x + inputPad;
        float leftInputW = separatorX - leftInputX - inputPad;
        float rightInputX = separatorX + inputPad;
        float rightInputW = rightBadgeMin.x - rightInputX - inputPad;

        leftInputW = ImMax(leftInputW, 48.f);
        rightInputW = ImMax(rightInputW, 48.f);

        ImVec4 leftAccent = ImVec4(g_activeAccent.x, g_activeAccent.y, g_activeAccent.z, 0.85f);
        ImVec4 rightAccent = ImVec4(0.93f, 0.38f, 0.46f, 0.95f);

        ImGui::SetCursorScreenPos(ImVec2(leftInputX, origin.y));
        ImGui::PushStyleColor(ImGuiCol_Text, leftAccent);
        ImGui::TextUnformatted(leftLabel);
        ImGui::PopStyleColor();

        ImGui::SetCursorScreenPos(ImVec2(rightInputX, origin.y));
        ImGui::PushStyleColor(ImGuiCol_Text, rightAccent);
        ImGui::TextUnformatted(rightLabel);
        ImGui::PopStyleColor();

        ImGui::PushID(id);

        ImGui::SetCursorScreenPos(leftBadgeMin);
        ImGui::InvisibleButton("##left_add", ImVec2(badgeWidth, fieldHeight));
        bool leftButtonHovered = ImGui::IsItemHovered();
        bool leftButtonHeld = ImGui::IsItemActive();
        leftAddTriggered = ImGui::IsItemClicked();

        ImGui::SetCursorScreenPos(ImVec2(leftInputX, capsuleMin.y));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.f, inputYOffset));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushItemWidth(leftInputW);
        ImGui::InputText("##left_filter", leftBuffer, leftBufferSize);
        bool leftInputHovered = ImGui::IsItemHovered();
        bool leftInputActive = ImGui::IsItemActive();
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);

        ImGui::SetCursorScreenPos(ImVec2(rightInputX, capsuleMin.y));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.f, inputYOffset));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushItemWidth(rightInputW);
        ImGui::InputText("##right_filter", rightBuffer, rightBufferSize);
        bool rightInputHovered = ImGui::IsItemHovered();
        bool rightInputActive = ImGui::IsItemActive();
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);

        ImGui::SetCursorScreenPos(rightBadgeMin);
        ImGui::InvisibleButton("##right_add", ImVec2(badgeWidth, fieldHeight));
        bool rightButtonHovered = ImGui::IsItemHovered();
        bool rightButtonHeld = ImGui::IsItemActive();
        rightAddTriggered = ImGui::IsItemClicked();

        if (leftButtonHovered || rightButtonHovered)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        splitter.SetCurrentChannel(dl, 0);

        ImVec4 baseFill = ImLerp(g_guiColorFrame, ImVec4(0.f, 0.f, 0.f, g_guiColorFrame.w), 0.2f);
        ImVec4 borderBase = ImLerp(g_guiColorFrame, g_guiColorAccent, 0.2f);
        float rounding = fieldHeight * 0.5f;
        dl->AddRectFilled(capsuleMin, capsuleMax,
            ImGui::ColorConvertFloat4ToU32(baseFill), rounding);

        if (leftInputHovered || leftInputActive)
        {
            ImVec4 hFill = ImLerp(baseFill, ImVec4(1.f, 1.f, 1.f, 0.15f), 0.15f);
            dl->AddRectFilled(leftBadgeMax, ImVec2(separatorX, capsuleMax.y),
                ImGui::ColorConvertFloat4ToU32(hFill));
        }
        if (rightInputHovered || rightInputActive)
        {
            ImVec4 hFill = ImLerp(baseFill, ImVec4(1.f, 1.f, 1.f, 0.15f), 0.15f);
            dl->AddRectFilled(ImVec2(separatorX, capsuleMin.y), rightBadgeMin,
                ImGui::ColorConvertFloat4ToU32(hFill));
        }

        if (leftButtonHeld || leftButtonHovered)
        {
            ImVec4 bFill = leftButtonHeld ? ImVec4(1.f, 1.f, 1.f, 0.18f) : ImVec4(1.f, 1.f, 1.f, 0.10f);
            dl->AddRectFilled(leftBadgeMin, leftBadgeMax,
                ImGui::ColorConvertFloat4ToU32(bFill), rounding,
                ImDrawFlags_RoundCornersLeft);
        }
        if (rightButtonHeld || rightButtonHovered)
        {
            ImVec4 bFill = rightButtonHeld ? ImVec4(1.f, 1.f, 1.f, 0.18f) : ImVec4(1.f, 1.f, 1.f, 0.10f);
            dl->AddRectFilled(rightBadgeMin, rightBadgeMax,
                ImGui::ColorConvertFloat4ToU32(bFill), rounding,
                ImDrawFlags_RoundCornersRight);
        }

        dl->AddRect(capsuleMin, capsuleMax,
            ImGui::ColorConvertFloat4ToU32(borderBase), rounding, 0, 1.1f);

        ImU32 divider = IM_COL32(86, 86, 100, 210);
        dl->AddLine(ImVec2(leftBadgeMax.x, capsuleMin.y + 6.f),
            ImVec2(leftBadgeMax.x, capsuleMax.y - 6.f), divider, 1.1f);
        dl->AddLine(ImVec2(separatorX, capsuleMin.y + 6.f),
            ImVec2(separatorX, capsuleMax.y - 6.f), divider, 1.1f);
        dl->AddLine(ImVec2(rightBadgeMin.x, capsuleMin.y + 6.f),
            ImVec2(rightBadgeMin.x, capsuleMax.y - 6.f), divider, 1.1f);

        splitter.SetCurrentChannel(dl, 1);
        float edgeGlyphInset = ImClamp(fieldHeight * 0.10f, 1.f, 2.2f);
        ImVec2 leftCenter(leftBadgeMin.x + badgeWidth * 0.5f + edgeGlyphInset, leftBadgeMin.y + fieldHeight * 0.5f);
        ImVec2 rightCenter(rightBadgeMin.x + badgeWidth * 0.5f - edgeGlyphInset, rightBadgeMin.y + fieldHeight * 0.5f);
        float half = fieldHeight * 0.18f;

        ImU32 plusColor = leftButtonHeld ? IM_COL32(255, 255, 255, 255)
            : leftButtonHovered ? IM_COL32(230, 230, 240, 255) : IM_COL32(180, 180, 195, 210);
        dl->AddLine(ImVec2(leftCenter.x - half, leftCenter.y), ImVec2(leftCenter.x + half, leftCenter.y), plusColor, 1.6f);
        dl->AddLine(ImVec2(leftCenter.x, leftCenter.y - half), ImVec2(leftCenter.x, leftCenter.y + half), plusColor, 1.6f);

        ImU32 crossColor = rightButtonHeld ? IM_COL32(255, 255, 255, 255)
            : rightButtonHovered ? IM_COL32(230, 230, 240, 255) : IM_COL32(180, 180, 195, 210);
        dl->AddLine(ImVec2(rightCenter.x - half, rightCenter.y - half), ImVec2(rightCenter.x + half, rightCenter.y + half), crossColor, 1.6f);
        dl->AddLine(ImVec2(rightCenter.x - half, rightCenter.y + half), ImVec2(rightCenter.x + half, rightCenter.y - half), crossColor, 1.6f);

        splitter.Merge(dl);
        ImGui::PopID();

        ImVec2 nextPos(origin.x, origin.y + rowHeight);
        ImGui::SetCursorScreenPos(nextPos);
        ImGui::Dummy(ImVec2(0.01f, 0.01f));
        ImGui::SetCursorScreenPos(nextPos);
        return rowHeight;
    }
    static bool EqualsIgnoreCase(const std::string& a, const std::string& b)
    {
        if (a.size() != b.size())
            return false;
        for (size_t i = 0; i < a.size(); ++i)
        {
            unsigned char lhs = static_cast<unsigned char>(a[i]);
            unsigned char rhs = static_cast<unsigned char>(b[i]);
            if (std::tolower(lhs) != std::tolower(rhs))
                return false;
        }
        return true;
    }
    static void AddCapsuleTokens(const std::string& raw, std::vector<CapsuleEntry>& target, const ImVec4& defaultColor)
    {
        auto trimEnds = [](std::string& s)
            {
                while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front())))
                    s.erase(s.begin());
                while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
                    s.pop_back();
            };

        size_t start = 0;
        while (start < raw.size())
        {
            size_t comma = raw.find(',', start);
            std::string token = raw.substr(start,
                comma == std::string::npos ? std::string::npos : comma - start);
            trimEnds(token);
            if (!token.empty() &&
                std::find_if(target.begin(), target.end(),
                    [&](const CapsuleEntry& entry) { return EqualsIgnoreCase(entry.label, token); }) == target.end())
            {
                target.push_back(CapsuleEntry{ token, defaultColor });
            }

            if (comma == std::string::npos)
                break;
            start = comma + 1;
        }
    }
    static float InputAddRow(const char* id, const char* label, char* buffer, size_t bufferSize,
        bool& addTriggered, const ImVec2& origin, float innerWidth)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImDrawListSplitter splitter;
        splitter.Split(dl, 2);
        splitter.SetCurrentChannel(dl, 1);

        const float fontSize = ImGui::GetFontSize();
        const float fieldHeight = fontSize + 10.f;
        const float rowHeight = fieldHeight + 8.f;

        float fieldWidth = ImClamp(innerWidth * 0.60f, 200.f, innerWidth - 24.f);
        float fieldX = origin.x + innerWidth - fieldWidth;
        if (fieldX < origin.x + 90.f)
            fieldX = origin.x + 90.f;

        const float buttonWidth = 36.f;
        const float buttonGap = 0.f;
        float inputWidth = ImMax(fieldWidth - buttonWidth - buttonGap, 80.f);
        float inputY = origin.y + (rowHeight - fieldHeight) * 0.5f;

        ImGui::SetCursorScreenPos(origin);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.64f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImGui::PushID(id);

        ImVec2 inputPos(fieldX, inputY);
        ImVec2 inputSize(inputWidth, fieldHeight);
        ImGui::SetCursorScreenPos(inputPos);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.f, (fieldHeight - fontSize) * 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushItemWidth(inputSize.x);
        ImGui::InputText("##filter_input", buffer, bufferSize);
        bool inputHovered = ImGui::IsItemHovered();
        bool inputActive = ImGui::IsItemActive();
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);

        ImVec2 buttonPos(inputPos.x + inputSize.x + buttonGap, inputY);
        ImVec2 buttonSize(buttonWidth, fieldHeight);
        ImGui::SetCursorScreenPos(buttonPos);
        ImGui::InvisibleButton("##add_button", buttonSize);
        bool hovered = ImGui::IsItemHovered();
        bool held = ImGui::IsItemActive();
        addTriggered = ImGui::IsItemClicked();

        if (hovered)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        ImVec4 baseFill = ImLerp(g_guiColorFrame, ImVec4(0.f, 0.f, 0.f, g_guiColorFrame.w), 0.25f);
        ImVec4 hoverFill = ImLerp(baseFill, g_guiColorAccent, 0.25f);
        ImVec4 activeFill = ImVec4(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 0.65f);

        bool anyHovered = inputHovered || hovered;
        bool anyHeld = inputActive || held;
        ImVec4 fill = baseFill;
        if (anyHovered)
            fill = ImLerp(fill, hoverFill, 0.8f);
        if (anyHeld)
            fill = ImLerp(fill, activeFill, 0.65f);

        ImVec4 borderBase = ImLerp(g_guiColorFrame, g_guiColorAccent, 0.2f);
        ImVec4 borderAccent = ImVec4(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 0.85f);
        ImVec4 borderCol = anyHeld ? ImLerp(borderBase, borderAccent, 0.6f)
            : anyHovered ? ImLerp(borderBase, borderAccent, 0.3f) : borderBase;

        ImVec2 combinedMin = inputPos;
        ImVec2 combinedMax(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y);
        float rounding = fieldHeight * 0.5f;
        splitter.SetCurrentChannel(dl, 0);
        dl->AddRectFilled(combinedMin, combinedMax, ImGui::ColorConvertFloat4ToU32(fill), rounding);
        dl->AddRect(combinedMin, combinedMax, ImGui::ColorConvertFloat4ToU32(borderCol), rounding, 0, 1.25f);
        splitter.SetCurrentChannel(dl, 1);
        ImVec2 separatorMin(buttonPos.x, buttonPos.y + 6.f);
        ImVec2 separatorMax(buttonPos.x, buttonPos.y + buttonSize.y - 6.f);
        dl->AddLine(separatorMin, separatorMax, IM_COL32(86, 86, 100, 210), 1.2f);

        ImVec2 center(buttonPos.x + buttonSize.x * 0.5f, buttonPos.y + buttonSize.y * 0.5f);
        float plusHalf = buttonSize.y * 0.24f;
        ImU32 plusColor = held ? IM_COL32(255, 255, 255, 255)
            : hovered ? IM_COL32(238, 238, 246, 255) : IM_COL32(216, 216, 226, 255);
        dl->AddLine(ImVec2(center.x - plusHalf, center.y), ImVec2(center.x + plusHalf, center.y), plusColor, 1.8f);
        dl->AddLine(ImVec2(center.x, center.y - plusHalf), ImVec2(center.x, center.y + plusHalf), plusColor, 1.8f);

        splitter.Merge(dl);
        ImGui::PopID();

        return rowHeight;
    }

    static float InputConfirmRow(const char* id, const char* label, char* buffer, size_t bufferSize,
        bool& confirmed, const ImVec2& origin, float innerWidth)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const float fontSize = ImGui::GetFontSize();
        const float fieldHeight = fontSize + 10.f;
        const float rowHeight = fieldHeight + 8.f;

        float fieldWidth = ImClamp(innerWidth * 0.60f, 200.f, innerWidth - 24.f);
        float fieldX = origin.x + innerWidth - fieldWidth;
        if (fieldX < origin.x + 90.f)
            fieldX = origin.x + 90.f;

        const float buttonWidth = 36.f;
        const float buttonGap = 0.f;
        float inputWidth = ImMax(fieldWidth - buttonWidth - buttonGap, 80.f);
        float inputY = origin.y + (rowHeight - fieldHeight) * 0.5f;

        /*ImGui::SetCursorScreenPos(origin);*/
        float textY = origin.y + (rowHeight - fontSize) * 0.5f;
        ImGui::SetCursorScreenPos(ImVec2(origin.x, textY));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.64f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImGui::PushID(id);

        ImVec2 inputPos(fieldX, inputY);
        ImVec2 inputSize(inputWidth, fieldHeight);
        ImVec2 buttonPos(inputPos.x + inputSize.x + buttonGap, inputY);
        ImVec2 buttonSize(buttonWidth, fieldHeight);

        bool hoverInputRect = ImGui::IsMouseHoveringRect(inputPos, add(inputPos,inputSize));
        bool hoverButtonRect = ImGui::IsMouseHoveringRect(buttonPos, add(buttonPos,buttonSize));
        bool anyHoveredPre = hoverInputRect || hoverButtonRect;
        bool anyHeldPre = anyHoveredPre && ImGui::IsMouseDown(ImGuiMouseButton_Left);

        ImVec4 baseFill = ImLerp(g_guiColorFrame, ImVec4(0.f, 0.f, 0.f, g_guiColorFrame.w), 0.25f);
        ImVec4 hoverFill = ImLerp(baseFill, g_guiColorAccent, 0.25f);
        ImVec4 activeFill = ImVec4(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 0.65f);

        ImVec4 fill = baseFill;
        if (anyHoveredPre)
            fill = ImLerp(fill, hoverFill, 0.8f);
        if (anyHeldPre)
            fill = ImLerp(fill, activeFill, 0.65f);

        ImVec4 borderBase = ImLerp(g_guiColorFrame, g_guiColorAccent, 0.2f);
        ImVec4 borderAccent = ImVec4(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 0.85f);
        ImVec4 borderColPre = anyHeldPre ? ImLerp(borderBase, borderAccent, 0.6f)
            : anyHoveredPre ? ImLerp(borderBase, borderAccent, 0.3f) : borderBase;

        ImVec2 combinedMin = inputPos;
        ImVec2 combinedMax(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y);
        float rounding = fieldHeight * 0.5f;
        dl->AddRectFilled(combinedMin, combinedMax, ImGui::ColorConvertFloat4ToU32(fill), rounding);
        dl->AddRect(combinedMin, combinedMax, ImGui::ColorConvertFloat4ToU32(borderColPre), rounding, 0, 1.1f);

        ImVec2 separatorMin(buttonPos.x, buttonPos.y + 6.f);
        ImVec2 separatorMax(buttonPos.x, buttonPos.y + buttonSize.y - 6.f);
        dl->AddLine(separatorMin, separatorMax, IM_COL32(86, 86, 100, 210), 1.1f);

        ImGui::SetCursorScreenPos(inputPos);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.f, (fieldHeight - fontSize) * 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
        ImGui::PushItemWidth(inputSize.x);
        ImGui::InputText("##confirm_input", buffer, bufferSize);
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(2);

        bool inputHovered = ImGui::IsItemHovered();
        bool inputActive = ImGui::IsItemActive();

        ImGui::SetCursorScreenPos(buttonPos);
        ImGui::InvisibleButton("##confirm_button", buttonSize);
        bool hovered = ImGui::IsItemHovered();
        bool held = ImGui::IsItemActive();
        confirmed = ImGui::IsItemClicked();

        if (hovered)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        bool anyHovered = inputHovered || hovered;
        bool anyHeld = inputActive || held;

        ImVec4 borderCol = anyHeld ? ImLerp(borderBase, borderAccent, 0.6f)
            : anyHovered ? ImLerp(borderBase, borderAccent, 0.3f) : borderBase;

        dl->AddRect(combinedMin, combinedMax, ImGui::ColorConvertFloat4ToU32(borderCol), rounding, 0, 1.1f);

        ImVec2 checkStart(buttonPos.x + buttonSize.x * 0.28f, buttonPos.y + buttonSize.y * 0.55f);
        ImVec2 checkMid(buttonPos.x + buttonSize.x * 0.42f, buttonPos.y + buttonSize.y * 0.70f);
        ImVec2 checkEnd(buttonPos.x + buttonSize.x * 0.72f, buttonPos.y + buttonSize.y * 0.30f);
        ImU32 checkColor = held ? IM_COL32(255, 255, 255, 255)
            : hovered ? IM_COL32(238, 238, 246, 255) : IM_COL32(216, 216, 226, 255);
        dl->AddLine(checkStart, checkMid, checkColor, 2.f);
        dl->AddLine(checkMid, checkEnd, checkColor, 2.f);

        ImGui::PopID();

        ImVec2 nextPos(origin.x, origin.y + rowHeight);
        ImGui::SetCursorScreenPos(nextPos);
        ImGui::Dummy(ImVec2(0.01f, 0.01f));
        ImGui::SetCursorScreenPos(nextPos);
        return rowHeight;
    }
    
    static void CustomCheckbox(const char* id, const char* label, bool& value, const ImVec2& pos)
    {
        ImGui::PushID(id);
        ImGui::SetCursorScreenPos(pos);
        ImGui::InvisibleButton("##checkbox", ImVec2(180.f, ImGui::GetFrameHeight()));
        if (ImGui::IsItemClicked())
            value = !value;

        ImVec2 center = ImVec2(pos.x + 10.f, pos.y + ImGui::GetFrameHeight() * 0.5f);
        float outer = 7.5f;
        float inner = value ? 4.0f : 0.f;

        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddCircleFilled(center, outer + 3.f, IM_COL32(20, 18, 26, 200), 36);
        dl->AddCircle(center, outer + 0.5f, IM_COL32(96, 70, 140, 200), 36, 1.1f);
        dl->AddCircleFilled(center, outer, value ? IM_COL32(150, 80, 220, 255) : IM_COL32(50, 50, 60, 255), 36);
        if (value)
            dl->AddCircleFilled(center, inner, IM_COL32(255, 255, 255, 240), 24);

        ImGui::SetCursorScreenPos(ImVec2(pos.x + 24.f, pos.y));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.86f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();
        ImGui::PopID();
    }

    static bool CustomSliderFloat(const char* id, float* v, float v_min, float v_max, const char* display_fmt, float width)
    {
        ImGui::PushID(id);

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float trackH = 4.0f;
        float thumb = 12.0f;
        ImVec2 size(width, thumb);

        ImGui::InvisibleButton("##slider", size);
        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();

        if (active || (hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left)))
        {
            float t = (ImGui::GetIO().MousePos.x - pos.x) / width;
            t = ImClamp(t, 0.0f, 1.0f);
            *v = v_min + (v_max - v_min) * t;
        }

        float t = (*v - v_min) / (v_max - v_min);
        t = ImClamp(t, 0.0f, 1.0f);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 trackMin(pos.x, pos.y + (thumb - trackH) * 0.5f);
        ImVec2 trackMax(pos.x + width, trackMin.y + trackH);

        dl->AddRectFilled(trackMin, trackMax, IM_COL32(38, 38, 43, 255), 5.f);
        dl->AddRectFilled(trackMin, ImVec2(trackMin.x + width * t, trackMax.y), IM_COL32(176, 176, 186, 255), 5.f);

        ImVec2 c(trackMin.x + width * t, pos.y + thumb * 0.5f);
        float d = 6.f;
        dl->AddQuadFilled(ImVec2(c.x, c.y - d), ImVec2(c.x + d, c.y), ImVec2(c.x, c.y + d), ImVec2(c.x - d, c.y),
            IM_COL32(255, 255, 255, 255));

        ImGui::PopID();
        return active;
    }
    static float DropdownRow(const char* id, const char* label, const char* const* options, int optionCount,
        int& currentIndex, const ImVec2& origin, float innerWidth)
    {
        const float fontSize = ImGui::GetFontSize();
        const float capsuleHeight = fontSize + 12.f;
        const float baseRowHeight = capsuleHeight + 8.f;
        const float optionHeight = fontSize + 14.f;
        const float optionSpacing = 4.f;
        const float listPadTop = 0.f;
        const float listPadBottom = 6.f;
        const float listPadX = 10.f;

        float capsuleWidth = ImClamp(innerWidth * 0.38f, 120.f, innerWidth - 88.f);
        float capsuleX = origin.x + innerWidth - capsuleWidth;
        if (capsuleX < origin.x + 110.f)
            capsuleX = origin.x + 110.f;
        float maxWidth = origin.x + innerWidth - capsuleX;

        ImGui::SetCursorScreenPos(origin);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.62f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        float capsuleY = origin.y + (baseRowHeight - capsuleHeight) * 0.5f;
        ImVec2 capsulePos(capsuleX, capsuleY);

        ImGui::PushID(id);
        ImGuiID stateId = ImGui::GetID("##dropdown_state");
        DropdownAnimState& state = g_dropdownStates[stateId];

        float expandWidth = 84.f;
        float displayWidth = state.open ? ImMin(capsuleWidth + expandWidth, maxWidth) : capsuleWidth;
        ImVec2 capsuleSize(displayWidth, capsuleHeight);

        ImGui::SetCursorScreenPos(capsulePos);
        bool pressed = ImGui::InvisibleButton("##capsule", capsuleSize);
        bool hovered = ImGui::IsItemHovered();
        bool held = ImGui::IsItemActive();
        if (pressed)
            state.open = !state.open;

        float dt = ImGui::GetIO().DeltaTime;
        float target = state.open ? 1.f : 0.f;
        float animSpeed = state.open ? 12.f : 32.f;
        state.anim += (target - state.anim) * ImClamp(dt * animSpeed, 0.f, 1.f);
        if (!state.open && state.anim < 0.02f)
            state.anim = 0.f;
        state.anim = ImSaturate(state.anim);
        float ease = state.anim * state.anim * (3.f - 2.f * state.anim);

        ImDrawList* dl = ImGui::GetWindowDrawList();

        ImVec4 baseFill = ImLerp(g_guiColorFrame, ImVec4(0.f, 0.f, 0.f, g_guiColorFrame.w), 0.2f);
        ImVec4 hoverFill = ImLerp(baseFill, g_guiColorAccent, 0.20f);
        ImVec4 accentCapsule(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 0.98f);

        ImVec4 fill = baseFill;
        if (state.open)
            fill = ImLerp(fill, hoverFill, ease * 0.25f);
        if (!state.open && hovered)
            fill = ImLerp(fill, hoverFill, 0.75f);
        if (held)
            fill = ImLerp(fill, accentCapsule, 0.35f);

        ImVec4 borderBase = ImLerp(g_guiColorFrame, g_guiColorAccent, 0.2f);
        ImVec4 borderAccent(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 0.98f);
        ImVec4 borderFill = ImLerp(borderBase, borderAccent, ease);
        if (!state.open && (hovered || held))
            borderFill = ImLerp(borderFill, borderAccent, 0.65f);

        float containerHeight = optionCount > 0
            ? listPadTop + listPadBottom + optionHeight * optionCount + optionSpacing * (optionCount - 1)
            : 0.f;
        float revealHeight = containerHeight * ease;
        float rounding = capsuleHeight * 0.5f;
        ImVec2 capsuleMax = add(capsulePos,capsuleSize);
        ImVec2 groupMax = ImVec2(capsulePos.x + displayWidth, capsulePos.y + capsuleHeight + revealHeight);
        ImVec4 containerFill = ImLerp(baseFill, g_guiColorFrame, 0.35f);

        if (containerHeight > 0.f)
        {
            ImGui::SetCursorScreenPos(origin);
            ImGui::Dummy(ImVec2(innerWidth, baseRowHeight + containerHeight));
        }

        if (revealHeight <= 0.001f || containerHeight <= 0.f)
        {
            dl->AddRectFilled(capsulePos, capsuleMax,
                ImGui::ColorConvertFloat4ToU32(fill), rounding);
            dl->AddRect(capsulePos, capsuleMax,
                ImGui::ColorConvertFloat4ToU32(borderFill), rounding, 0, 1.15f);
        }
        else
        {
            dl->AddRectFilled(capsulePos, groupMax,
                ImGui::ColorConvertFloat4ToU32(containerFill), rounding);
            dl->AddRectFilled(capsulePos, ImVec2(capsulePos.x + displayWidth, capsulePos.y + capsuleHeight),
                ImGui::ColorConvertFloat4ToU32(fill), rounding, ImDrawFlags_RoundCornersTop);
            dl->AddRect(capsulePos, groupMax,
                ImGui::ColorConvertFloat4ToU32(borderFill), rounding, 0, 1.15f);
        }

        ImVec2 arrowCenter(capsulePos.x + 18.f, capsulePos.y + capsuleHeight * 0.5f - 0.5f);
        float arrowHalfWidth = 4.5f;
        float arrowHalfHeight = 3.2f;
        bool arrowUp = (state.open && ease > 0.25f);
        if (arrowUp)
            dl->AddTriangleFilled(
                ImVec2(arrowCenter.x - arrowHalfWidth, arrowCenter.y + arrowHalfHeight),
                ImVec2(arrowCenter.x + arrowHalfWidth, arrowCenter.y + arrowHalfHeight),
                ImVec2(arrowCenter.x, arrowCenter.y - arrowHalfHeight),
                IM_COL32(255, 255, 255, 255));
        else
            dl->AddTriangleFilled(
                ImVec2(arrowCenter.x - arrowHalfWidth, arrowCenter.y - arrowHalfHeight),
                ImVec2(arrowCenter.x + arrowHalfWidth, arrowCenter.y - arrowHalfHeight),
                ImVec2(arrowCenter.x, arrowCenter.y + arrowHalfHeight),
                IM_COL32(225, 225, 235, 255));

        const char* currentLabel = (currentIndex >= 0 && currentIndex < optionCount) ? options[currentIndex] : "";
        ImVec2 textSize = ImGui::CalcTextSize(currentLabel);
        float textAreaStart = capsulePos.x + 32.f;
        float textAreaEnd = capsulePos.x + capsuleSize.x - 16.f;
        float textAreaWidth = ImMax(textAreaEnd - textAreaStart, 0.f);
        float textX = textAreaStart + (textAreaWidth - textSize.x) * 0.5f;
        ImVec2 textPos(textX, capsulePos.y + (capsuleHeight - textSize.y) * 0.5f - 0.5f);
        dl->AddText(textPos, IM_COL32(240, 240, 248, 255), currentLabel);

        ImVec2 containerPos(capsulePos.x, capsulePos.y + capsuleHeight);
        ImVec2 containerSize(displayWidth, containerHeight);
        if (containerHeight > 0.f)
        {
            ImGui::SetCursorScreenPos(capsulePos);
            ImGui::Dummy(ImVec2(displayWidth, baseRowHeight + containerHeight));
        }
        if (revealHeight > 0.001f && containerHeight > 0.f)
        {
            ImVec2 clipMax(containerPos.x + containerSize.x, containerPos.y + revealHeight);
            ImGui::PushClipRect(containerPos, clipMax, true);
            float optionY = containerPos.y + listPadTop;
            const float optionEdgeInset = 10.f;
            const float visibleBottom = containerPos.y + revealHeight;

            for (int i = 0; i < optionCount; ++i)
            {
                if (optionY >= visibleBottom)
                    break;
                ImGui::PushID(i);
                const char* optionLabel = options[i];
                ImVec2 optionTextSize = ImGui::CalcTextSize(optionLabel);

                float optionWidth = containerSize.x - optionEdgeInset * 2.f;
                optionWidth = ImMax(optionWidth, optionTextSize.x + 28.f);
                float maxOptionWidth = ImMax(40.f, containerSize.x - optionEdgeInset * 0.25f);
                optionWidth = ImClamp(optionWidth, 40.f, maxOptionWidth);
                float optionX = containerPos.x + (containerSize.x - optionWidth) * 0.5f;
                ImVec2 optionPos(optionX, optionY);
                ImVec2 optionSize(optionWidth, optionHeight);

                ImGui::SetCursorScreenPos(optionPos);
                ImGui::InvisibleButton("##option", optionSize);
                bool allowInput = state.open && ease > 0.2f;
                bool optionHovered = allowInput && ImGui::IsItemHovered();
                bool optionHeld = allowInput && ImGui::IsItemActive();
                bool selected = (i == currentIndex);

                ImVec2 drawMin = ImGui::GetItemRectMin();
                ImVec2 drawMax = ImGui::GetItemRectMax();
                float optionRound = 0.f;

                ImVec4 textColor = selected
                    ? ImVec4(g_activeAccent.x, g_activeAccent.y, g_activeAccent.z, 1.f)
                    : ImVec4(0.78f, 0.78f, 0.86f, 1.f);
                if (optionHovered && !selected)
                    textColor = ImVec4(1.f, 1.f, 1.f, 1.f);
                if (selected)
                    textColor = ImVec4(1.f, 1.f, 1.f, 1.f);

                ImVec2 optionTextPos(drawMin.x + (optionSize.x - optionTextSize.x) * 0.5f,
                    drawMin.y + (optionHeight - optionTextSize.y) * 0.5f - 0.5f);
                dl->AddText(optionTextPos, ImGui::ColorConvertFloat4ToU32(textColor), optionLabel);

                if (allowInput && ImGui::IsItemClicked())
                {
                    currentIndex = i;
                    state.open = false;
                }

                if (i < optionCount - 1)
                {
                    ImVec2 sepStart(containerPos.x + optionEdgeInset, optionY + optionHeight + optionSpacing * 0.5f);
                    ImVec2 sepEnd(containerPos.x + containerSize.x - optionEdgeInset, sepStart.y);
                    dl->AddLine(sepStart, sepEnd, IM_COL32(72, 72, 82, 180), 1.f);
                }

                optionY += optionHeight;
                if (i < optionCount - 1)
                    optionY += optionSpacing;
                ImGui::PopID();
            }
            ImGui::PopClipRect();
        }

        ImVec2 nextPos(origin.x, origin.y + baseRowHeight + revealHeight);
        ImVec2 dummySize(displayWidth, baseRowHeight + revealHeight);
        ImGui::SetCursorScreenPos(origin);
        ImGui::Dummy(dummySize);
        ImGui::PopID();

        return baseRowHeight + revealHeight;
    }
    static float SliderRow(const char* id, const char* label, float* value, float v_min, float v_max, const char* display_fmt,
        const ImVec2& origin, float innerWidth, bool brightLabel = false);
    static float SliderRow(const char* id, const char* label, float* value, float v_min, float v_max, const char* display_fmt,
        const ImVec2& origin, float innerWidth, bool brightLabel)
    {
        const float sliderHeight = 12.f;
        const float rowHeight = ImGui::GetFontSize() + sliderHeight + 10.f;
        float sliderWidth = ImClamp(innerWidth * 0.55f, 140.f, innerWidth - 80.f);
        float sliderX = origin.x + innerWidth - sliderWidth;
        if (sliderX < origin.x + 90.f)
            sliderX = origin.x + 90.f;
        float sliderY = origin.y + ImGui::GetFontSize() + 4.f;
        float labelY = sliderY + (sliderHeight - ImGui::GetFontSize()) * 0.5f;

        ImGui::SetCursorScreenPos(ImVec2(origin.x, labelY));
        ImVec4 labelColor = brightLabel ? ImVec4(1.f, 1.f, 1.f, 0.92f) : ImVec4(1.f, 1.f, 1.f, 0.64f);
        ImGui::PushStyleColor(ImGuiCol_Text, labelColor);
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        char valueBuf[32]{};
        _snprintf_s(valueBuf, sizeof(valueBuf), _TRUNCATE, display_fmt ? display_fmt : "%.2f", *value);
        ImVec2 valueSize = ImGui::CalcTextSize(valueBuf);
        float valueX = sliderX + sliderWidth - valueSize.x;
        float valueY = origin.y;
        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddText(ImVec2(valueX, valueY), IM_COL32(255, 255, 255, 220), valueBuf);

        ImGui::SetCursorScreenPos(ImVec2(sliderX, sliderY));
        CustomSliderFloat(id, value, v_min, v_max, display_fmt, sliderWidth);

        return rowHeight;
    }
    static void SetNextHeaderSplitBadge(const char* cardId, int* value, const char* leftLabel, const char* rightLabel, bool* linkedBool = nullptr)
    {
        g_pendingSplitBadge.cardId = cardId;
        g_pendingSplitBadge.value = value;
        g_pendingSplitBadge.linkedBool = linkedBool;
        g_pendingSplitBadge.leftLabel = leftLabel;
        g_pendingSplitBadge.rightLabel = rightLabel;
    }
    static float CapsuleListRow(const char* id, std::vector<CapsuleEntry>& entries,
        const ImVec2& origin, float innerWidth, int* clickedIndex = nullptr,
        bool allowContextEdit = true, bool edgeCloseStyle = false)
    {
        if (entries.empty())
            return 0.f;

        if (clickedIndex)
            *clickedIndex = -1;

        ImDrawList* dl = ImGui::GetWindowDrawList();
        const float fontSize = ImGui::GetFontSize();
        const float capsuleHeight = fontSize + 10.f;
        const float spacingX = 8.f;
        const float spacingY = 6.f;
        const float textPad = 12.f;
        const float closePad = 4.f;

        float currentX = origin.x;
        float currentY = origin.y;
        float maxX = origin.x + innerWidth;
        float totalHeight = capsuleHeight;
        int removeIndex = -1;

        ImGui::PushID(id);
        for (int i = 0; i < (int)entries.size(); ++i)
        {
            CapsuleEntry& entry = entries[i];
            ImVec2 textSize = ImGui::CalcTextSize(entry.label.c_str());
            float closeSize = edgeCloseStyle ? ImMax(7.f, capsuleHeight - 14.f) : (capsuleHeight - 12.f);
            float closeAreaWidth = edgeCloseStyle ? ImClamp(capsuleHeight - 2.f, 16.f, 22.f) : (closePad * 2.f + closeSize);
            float capsuleWidth = textPad * 2.f + textSize.x + closeAreaWidth;
            capsuleWidth = ImClamp(capsuleWidth, closeSize + textPad * 2.f, innerWidth);

            if (currentX + capsuleWidth > maxX && currentX > origin.x)
            {
                currentX = origin.x;
                currentY += capsuleHeight + spacingY;
                totalHeight += capsuleHeight + spacingY;
            }

            ImVec2 capsuleMin(currentX, currentY);
            ImVec2 capsuleMax(currentX + capsuleWidth, currentY + capsuleHeight);
            float rounding = capsuleHeight * 0.5f;

            ImGui::PushID(i);
            ImGui::SetCursorScreenPos(capsuleMin);
            ImGui::InvisibleButton("##capsule_hit", ImVec2(capsuleWidth, capsuleHeight));
            bool capsuleHovered = ImGui::IsItemHovered();
            bool leftClicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
            bool rightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
            ImGui::SetItemAllowOverlap();
            if (capsuleHovered)
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            if (allowContextEdit && rightClicked)
            {
                ImGui::OpenPopup(XS("color_popup"));
            }

            ImVec4 tint = entry.color;
            tint.w = 1.f;
            auto blendChannel = [](float base, float tintValue, float mix)
                {
                    return ImClamp(base + tintValue * mix, 0.f, 1.f);
                };

            float hoverBoost = capsuleHovered ? 0.05f : 0.f;
            ImVec4 baseCapsule = ImLerp(g_guiColorFrame, ImVec4(0.f, 0.f, 0.f, g_guiColorFrame.w), 0.2f);
            ImVec4 fill = ImVec4(
                blendChannel(baseCapsule.x + hoverBoost, tint.x, 0.55f),
                blendChannel(baseCapsule.y + hoverBoost, tint.y, 0.55f),
                blendChannel(baseCapsule.z + hoverBoost, tint.z, 0.55f),
                baseCapsule.w);
            ImVec4 border = ImVec4(
                blendChannel(g_guiColorFrame.x + hoverBoost * 0.4f, tint.x, 0.55f),
                blendChannel(g_guiColorFrame.y + hoverBoost * 0.4f, tint.y, 0.55f),
                blendChannel(g_guiColorFrame.z + hoverBoost * 0.4f, tint.z, 0.55f),
                0.90f);

            dl->AddRectFilled(capsuleMin, capsuleMax,
                ImGui::ColorConvertFloat4ToU32(fill), rounding);
            dl->AddRect(capsuleMin, capsuleMax,
                ImGui::ColorConvertFloat4ToU32(border), rounding, 0, 1.1f);

            ImVec2 textPos(capsuleMin.x + textPad,
                capsuleMin.y + (capsuleHeight - textSize.y) * 0.5f - 0.5f);
            dl->AddText(textPos, IM_COL32(240, 240, 248, 255), entry.label.c_str());

            ImVec2 closeAreaMin = edgeCloseStyle
                ? ImVec2(capsuleMax.x - closeAreaWidth, capsuleMin.y)
                : ImVec2(capsuleMax.x - closePad * 2.f - closeSize, capsuleMin.y + (capsuleHeight - closeSize) * 0.5f);
            ImVec2 closeAreaSize = edgeCloseStyle
                ? ImVec2(closeAreaWidth, capsuleHeight)
                : ImVec2(closePad * 2.f + closeSize, closeSize);
            ImVec2 closePos = edgeCloseStyle
                ? ImVec2(closeAreaMin.x + (closeAreaSize.x - closeSize) * 0.5f,
                    closeAreaMin.y + (closeAreaSize.y - closeSize) * 0.5f)
                : ImVec2(closeAreaMin.x + closePad, closeAreaMin.y);
            if (edgeCloseStyle)
                closePos.x -= ImClamp(closeAreaWidth * 0.12f, 1.f, 2.f);

            ImGui::SetCursorScreenPos(closeAreaMin);
            ImGui::InvisibleButton("##remove", closeAreaSize);
            bool hovered = ImGui::IsItemHovered();
            bool held = ImGui::IsItemActive();
            bool removeClicked = false;
            if (hovered)
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            if (ImGui::IsItemClicked())
            {
                removeIndex = i;
                removeClicked = true;
                ImGui::CloseCurrentPopup();
            }

            if (leftClicked && !removeClicked && clickedIndex)
                *clickedIndex = i;

            if (edgeCloseStyle)
            {
                ImVec4 closeFill = held ? ImVec4(1.f, 1.f, 1.f, 0.18f)
                    : hovered ? ImVec4(1.f, 1.f, 1.f, 0.10f) : ImVec4(0.f, 0.f, 0.f, 0.f);
                if (closeFill.w > 0.f)
                {
                    dl->AddRectFilled(closeAreaMin, capsuleMax,
                        ImGui::ColorConvertFloat4ToU32(closeFill), rounding,
                        ImDrawFlags_RoundCornersRight);
                }
                dl->AddLine(ImVec2(closeAreaMin.x, capsuleMin.y + 6.f),
                    ImVec2(closeAreaMin.x, capsuleMax.y - 6.f), IM_COL32(86, 86, 100, 210), 1.1f);
            }

            ImU32 closeColor = held ? IM_COL32(255, 255, 255, 255)
                : hovered ? IM_COL32(240, 240, 250, 255) : IM_COL32(190, 190, 200, 255);
            float half = closeSize * 0.24f;
            float cx = closePos.x + closeSize * 0.5f;
            float cy = closePos.y + closeSize * 0.5f;
            dl->AddLine(ImVec2(cx - half, cy - half), ImVec2(cx + half, cy + half), closeColor, 1.1f);
            dl->AddLine(ImVec2(cx - half, cy + half), ImVec2(cx + half, cy - half), closeColor, 1.1f);

            ImGui::SetNextWindowPos(ImGui::GetMousePos(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (allowContextEdit && ImGui::BeginPopup(XS("color_popup")))
            {
                char tmp[128];
                _snprintf_s(tmp, sizeof(tmp), _TRUNCATE, "%s", entry.label.c_str());

                ImGui::TextUnformatted(XS("Name"));
                ImGui::PushItemWidth(200.f);
                if (ImGui::InputText("##capsule_name", tmp, IM_ARRAYSIZE(tmp)))
                {
                    entry.label = tmp;
                }
                ImGui::PopItemWidth();

                ImGui::Separator();
                ImGui::PushItemWidth(180.f);
                ImVec4 color = entry.color;
                if (ImGui::ColorPicker4("##capsule_color", &color.x,
                    ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_DisplayRGB))
                {
                    color.w = 1.f;
                    entry.color = color;
                }
                ImGui::PopItemWidth();
                if (ImGui::Button(XS("Close##color_popup"), ImVec2(80.f, 0.f)))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            ImGui::PopID();

            currentX += capsuleWidth + spacingX;
        }
        ImGui::PopID();

        if (removeIndex >= 0 && removeIndex < (int)entries.size())
            entries.erase(entries.begin() + removeIndex);

        return totalHeight;
    }

    static void FormatColorHex(char* buffer, size_t size, const ImVec4& color)
    {
        if (!buffer || !size)
            return;
        int r = static_cast<int>(ImClamp(color.x, 0.f, 1.f) * 255.f + 0.5f);
        int g = static_cast<int>(ImClamp(color.y, 0.f, 1.f) * 255.f + 0.5f);
        int b = static_cast<int>(ImClamp(color.z, 0.f, 1.f) * 255.f + 0.5f);
        int a = static_cast<int>(ImClamp(color.w, 0.f, 1.f) * 255.f + 0.5f);
        _snprintf_s(buffer, size, _TRUNCATE, "#%02X%02X%02X%02X", r, g, b, a);
    }

    static bool ParseColorHex(const char* text, ImVec4& outColor)
    {
        if (!text)
            return false;

        const char* p = text;
        while (*p && std::isspace(static_cast<unsigned char>(*p)))
            ++p;
        if (*p == '#')
            ++p;

        char digits[9]{};
        size_t len = 0;
        while (*p && len < 8)
        {
            if (std::isspace(static_cast<unsigned char>(*p)))
            {
                ++p;
                continue;
            }
            if (!std::isxdigit(static_cast<unsigned char>(*p)))
                return false;
            digits[len++] = static_cast<char>(std::toupper(static_cast<unsigned char>(*p)));
            ++p;
        }
        while (*p && std::isspace(static_cast<unsigned char>(*p)))
            ++p;
        if (*p != '\0')
            return false;

        if (len != 6 && len != 8)
            return false;

        if (len == 6)
        {
            digits[len++] = 'F';
            digits[len++] = 'F';
        }
        digits[len] = '\0';

        auto hexNibble = [](char c) -> int
            {
                if (c >= '0' && c <= '9') return c - '0';
                if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
                return -1;
            };
        auto hexByte = [&](char hi, char lo) -> int
            {
                int h = hexNibble(hi);
                int l = hexNibble(lo);
                if (h < 0 || l < 0)
                    return -1;
                return (h << 4) | l;
            };

        int r = hexByte(digits[0], digits[1]);
        int g = hexByte(digits[2], digits[3]);
        int b = hexByte(digits[4], digits[5]);
        int a = hexByte(digits[6], digits[7]);
        if (r < 0 || g < 0 || b < 0 || a < 0)
            return false;

        const float inv = 1.0f / 255.0f;
        outColor.x = r * inv;
        outColor.y = g * inv;
        outColor.z = b * inv;
        outColor.w = a * inv;
        return true;
    }
    static std::unordered_map<ImGuiID, std::array<char, 16>> g_colorHexInputs;
    static float ColorButtonRow(const char* id, const char* label, ImVec4& color,
        const ImVec2& origin, float innerWidth, bool enablePicker = true)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const float fontSize = ImGui::GetFontSize();
        const float buttonSize = ImClamp(fontSize + 2.f, 16.f, 24.f);
        const float rowHeight = buttonSize + 8.f;

        float buttonX = origin.x + innerWidth - buttonSize;
        if (buttonX < origin.x + 90.f)
            buttonX = origin.x + 90.f;
        float buttonY = origin.y + (rowHeight - buttonSize) * 0.5f;

        ImGui::SetCursorScreenPos(origin);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.68f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImGui::PushID(id);
        ImVec2 buttonPos(buttonX, buttonY);
        ImVec2 buttonSizeVec(buttonSize, buttonSize);
        ImGui::SetCursorScreenPos(buttonPos);
        ImGui::InvisibleButton("##color_button", buttonSizeVec);
        bool hovered = ImGui::IsItemHovered();
        bool held = ImGui::IsItemActive();
        if (hovered)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        if (enablePicker && ImGui::IsItemClicked())
            ImGui::OpenPopup(XS("picker"));

        ImVec4 preview = color;
        float rounding = ImClamp(buttonSize * 0.35f, 3.f, 8.f);
        ImVec2 previewMin = buttonPos;
        ImVec2 previewMax = add(buttonPos,buttonSizeVec);
#ifdef ImDrawFlags_RoundCornersAll
        ImGui::RenderColorRectWithAlphaCheckerboard(dl, previewMin, previewMax,
            ImGui::GetColorU32(preview), 8.f, ImVec2(0.f, 0.f), rounding, ImDrawFlags_RoundCornersAll);
#else
        ImGui::RenderColorRectWithAlphaCheckerboard(dl, previewMin, previewMax,
            ImGui::GetColorU32(preview), 8.f, ImVec2(0.f, 0.f), rounding);
#endif

        ImVec4 overlay = ImVec4(1.f, 1.f, 1.f, hovered ? 0.08f : 0.f);
        if (held)
            overlay = ImVec4(1.f, 1.f, 1.f, 0.16f);
        if (overlay.w > 0.f)
            dl->AddRectFilled(previewMin, previewMax,
                ImGui::ColorConvertFloat4ToU32(overlay), rounding);

        ImVec4 baseBorder = g_guiColorOutline;
        ImVec4 highlightEdge = ImVec4(
            ImClamp(g_guiColorHighlight.x, 0.f, 1.f),
            ImClamp(g_guiColorHighlight.y, 0.f, 1.f),
            ImClamp(g_guiColorHighlight.z, 0.f, 1.f),
            ImClamp(g_guiColorHighlight.w + 0.28f, 0.f, 1.f));
        ImVec4 border = (hovered || held)
            ? ImLerp(baseBorder, highlightEdge, 0.65f)
            : baseBorder;
        ImVec2 borderInset(1.8f, 1.8f);
        ImVec2 borderMin = sub(previewMin,borderInset);
        ImVec2 borderMax = add(previewMax,borderInset);
        float borderRounding = ImClamp(rounding + 1.65f, rounding + 1.2f, rounding + 2.0f);
        dl->AddRect(borderMin, borderMax,
            ImGui::ColorConvertFloat4ToU32(border), borderRounding, 0, 1.8f);

        ImVec2 popupPos(buttonPos.x + buttonSizeVec.x * 0.5f,
            buttonPos.y + buttonSizeVec.y + 8.f);
        if (enablePicker)
        {
            ImGui::SetNextWindowPos(popupPos, ImGuiCond_Appearing, ImVec2(0.5f, 0.f));
            if (ImGui::BeginPopup(XS("picker")))
            {
                ImGui::TextUnformatted(label);
                ImGui::Separator();
                ImGui::PushItemWidth(240.f);

                ImVec4 temp = color;
                ImGuiID hexId = ImGui::GetID("hex_input");
                auto& hexBuffer = g_colorHexInputs[hexId];
                if (ImGui::IsWindowAppearing() || hexBuffer[0] == '\0')
                    FormatColorHex(hexBuffer.data(), hexBuffer.size(), temp);

                ImGui::TextUnformatted(XS("Hex Code"));
                if (ImGui::InputText("##color_hex", hexBuffer.data(), hexBuffer.size(),
                    ImGuiInputTextFlags_CharsUppercase))
                {

                }
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    ImVec4 parsed;
                    if (ParseColorHex(hexBuffer.data(), parsed))
                    {
                        temp = parsed;
                        color = parsed;
                        FormatColorHex(hexBuffer.data(), hexBuffer.size(), color);
                    }
                    else
                    {
                        FormatColorHex(hexBuffer.data(), hexBuffer.size(), color);
                    }
                }

                ImGui::Spacing();
                if (ImGui::ColorPicker4("##color_picker", &temp.x,
                    ImGuiColorEditFlags_DisplayRGB |
                    ImGuiColorEditFlags_AlphaBar |
                    ImGuiColorEditFlags_NoSidePreview |
                    ImGuiColorEditFlags_NoSmallPreview |
                    ImGuiColorEditFlags_AlphaPreviewHalf))
                {
                    color = temp;
                    FormatColorHex(hexBuffer.data(), hexBuffer.size(), color);
                }
                ImGui::PopItemWidth();

                ImGui::Spacing();
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.f, 10.f));
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.f, 6.f));
                ImGui::BeginGroup();
                if (ImGui::Button(XS("Copy##color_picker"), ImVec2(96.f, 28.f)))
                {
                    FormatColorHex(hexBuffer.data(), hexBuffer.size(), color);
                    ImGui::SetClipboardText(hexBuffer.data());
                }
                ImGui::SameLine();
                if (ImGui::Button(XS("Close##color_picker"), ImVec2(96.f, 28.f)))
                    ImGui::CloseCurrentPopup();
                ImGui::EndGroup();
                ImGui::PopStyleVar(2);
                ImGui::Dummy(ImVec2(0.f, 2.f));
                ImGui::EndPopup();
            }
        }
        ImGui::PopID();

        ImVec2 nextPos(origin.x, origin.y + rowHeight);
        ImGui::SetCursorScreenPos(origin);
        ImGui::Dummy(ImVec2(innerWidth, rowHeight));
        return rowHeight;
    }

    static float PathInputRow(const char* id, const char* label, char* buffer, size_t bufferSize,
        const ImVec2& origin, float innerWidth)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImDrawListSplitter splitter;
        splitter.Split(dl, 2);
        splitter.SetCurrentChannel(dl, 1);

        const float fontSize = ImGui::GetFontSize();
        const float fieldHeight = fontSize + 10.f;
        const float rowHeight = fieldHeight + 8.f;

        float fieldWidth = ImClamp(innerWidth * 0.64f, 220.f, innerWidth - 20.f);
        float fieldX = origin.x + innerWidth - fieldWidth;
        if (fieldX < origin.x + 90.f)
            fieldX = origin.x + 90.f;
        float fieldY = origin.y + (rowHeight - fieldHeight) * 0.5f;

        ImGui::SetCursorScreenPos(origin);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.64f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImGui::PushID(id);
        ImVec2 fieldPos(fieldX, fieldY);
        ImVec2 fieldSize(fieldWidth, fieldHeight);

        ImGui::SetCursorScreenPos(fieldPos);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.f, (fieldHeight - fontSize) * 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushItemWidth(fieldSize.x);
        ImGui::InputText("##path", buffer, bufferSize);
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);

        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();

        ImVec4 baseFill = ImVec4(0.12f, 0.12f, 0.16f, 1.f);
        ImVec4 hoverFill = ImVec4(0.18f, 0.18f, 0.24f, 1.f);
        ImVec4 activeFill = ImVec4(g_activeAccent.x, g_activeAccent.y, g_activeAccent.z, 0.45f);
        ImVec4 fill = baseFill;
        if (hovered)
            fill = ImLerp(fill, hoverFill, 0.7f);
        if (active)
            fill = ImLerp(fill, activeFill, 0.5f);

        ImVec4 borderBase = ImVec4(0.30f, 0.30f, 0.38f, 0.85f);
        ImVec4 borderAccent = ImVec4(g_activeAccent.x, g_activeAccent.y, g_activeAccent.z, 0.70f);
        ImVec4 borderCol = active ? ImLerp(borderBase, borderAccent, 0.6f)
            : hovered ? ImLerp(borderBase, borderAccent, 0.3f) : borderBase;

        splitter.SetCurrentChannel(dl, 0);
        float rounding = fieldHeight * 0.5f;
        dl->AddRectFilled(fieldPos, add(fieldPos,fieldSize),
            ImGui::ColorConvertFloat4ToU32(fill), rounding);
        dl->AddRect(fieldPos, add(fieldPos,fieldSize),
            ImGui::ColorConvertFloat4ToU32(borderCol), rounding, 0, 1.1f);

        splitter.Merge(dl);
        ImGui::PopID();

        ImVec2 nextPos(origin.x, origin.y + rowHeight);
        ImGui::SetCursorScreenPos(origin);
        ImGui::Dummy(ImVec2(innerWidth, rowHeight));
        return rowHeight;
    }

    static float ProfilePathRow(const char* id, const char* label, char* buffer, size_t bufferSize,
        char leftGlyph, bool& leftTriggered, bool& applyTriggered,
        const ImVec2& origin, float innerWidth)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImDrawListSplitter splitter;
        splitter.Split(dl, 2);
        splitter.SetCurrentChannel(dl, 1);

        leftTriggered = false;
        applyTriggered = false;

        const float fontSize = ImGui::GetFontSize();
        const float fieldHeight = fontSize + 10.f;
        const float rowHeight = fieldHeight + 8.f;

        float fieldWidth = ImClamp(innerWidth * 0.64f, 220.f, innerWidth - 20.f);
        float fieldX = origin.x + innerWidth - fieldWidth;
        if (fieldX < origin.x + 90.f)
            fieldX = origin.x + 90.f;
        float fieldY = origin.y + (rowHeight - fieldHeight) * 0.5f;

        const float buttonWidth = 32.f;
        const float buttonGap = 0.f;
        float inputWidth = ImMax(fieldWidth - (buttonWidth * 2.f) - (buttonGap * 2.f), 80.f);

        ImGui::SetCursorScreenPos(origin);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.64f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImGui::PushID(id);

        ImVec2 leftButtonPos(fieldX, fieldY);
        ImVec2 buttonSize(buttonWidth, fieldHeight);
        ImGui::SetCursorScreenPos(leftButtonPos);
        ImGui::InvisibleButton("##left_button", buttonSize);
        bool leftHovered = ImGui::IsItemHovered();
        bool leftHeld = ImGui::IsItemActive();
        leftTriggered = ImGui::IsItemClicked();

        ImVec2 inputPos(fieldX + buttonWidth + buttonGap, fieldY);
        ImVec2 inputSize(inputWidth, fieldHeight);
        ImGui::SetCursorScreenPos(inputPos);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.f, (fieldHeight - fontSize) * 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushItemWidth(inputSize.x);
        ImGui::InputText("##profile_path", buffer, bufferSize);
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);

        bool inputHovered = ImGui::IsItemHovered();
        bool inputActive = ImGui::IsItemActive();

        ImVec2 rightButtonPos(inputPos.x + inputSize.x + buttonGap, fieldY);
        ImGui::SetCursorScreenPos(rightButtonPos);
        ImGui::InvisibleButton("##apply_button", buttonSize);
        bool rightHovered = ImGui::IsItemHovered();
        bool rightHeld = ImGui::IsItemActive();
        applyTriggered = ImGui::IsItemClicked();

        if (leftHovered || rightHovered)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        bool anyHovered = inputHovered || leftHovered || rightHovered;
        bool anyHeld = inputActive || leftHeld || rightHeld;

        ImVec4 baseFill = ImLerp(g_guiColorFrame, ImVec4(0.f, 0.f, 0.f, g_guiColorFrame.w), 0.25f);
        ImVec4 hoverFill = ImLerp(baseFill, g_guiColorAccent, 0.25f);
        ImVec4 activeFill = ImVec4(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 0.65f);

        ImVec4 fill = baseFill;
        if (anyHovered)
            fill = ImLerp(fill, hoverFill, 0.8f);
        if (anyHeld)
            fill = ImLerp(fill, activeFill, 0.65f);

        ImVec4 borderBase = ImLerp(g_guiColorFrame, g_guiColorAccent, 0.2f);
        ImVec4 borderAccent = ImVec4(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 0.85f);
        ImVec4 borderCol = anyHeld ? ImLerp(borderBase, borderAccent, 0.6f)
            : anyHovered ? ImLerp(borderBase, borderAccent, 0.3f) : borderBase;

        ImVec2 combinedMin(leftButtonPos);
        ImVec2 combinedMax(rightButtonPos.x + buttonSize.x, rightButtonPos.y + buttonSize.y);
        float rounding = fieldHeight * 0.5f;

        splitter.SetCurrentChannel(dl, 0);
        dl->AddRectFilled(combinedMin, combinedMax, ImGui::ColorConvertFloat4ToU32(fill), rounding);
        dl->AddRect(combinedMin, combinedMax, ImGui::ColorConvertFloat4ToU32(borderCol), rounding, 0, 1.1f);

        ImVec2 leftSepMin(leftButtonPos.x + buttonSize.x, leftButtonPos.y + 6.f);
        ImVec2 leftSepMax(leftSepMin.x, leftButtonPos.y + buttonSize.y - 6.f);
        ImVec2 rightSepMin(rightButtonPos.x, rightButtonPos.y + 6.f);
        ImVec2 rightSepMax(rightSepMin.x, rightButtonPos.y + buttonSize.y - 6.f);
        dl->AddLine(leftSepMin, leftSepMax, IM_COL32(86, 86, 100, 210), 1.1f);
        dl->AddLine(rightSepMin, rightSepMax, IM_COL32(86, 86, 100, 210), 1.1f);

        ImU32 glyphColorLeft = leftHeld ? IM_COL32(255, 255, 255, 255)
            : leftHovered ? IM_COL32(238, 238, 246, 255) : IM_COL32(216, 216, 226, 255);
        char glyphStr[2] = { leftGlyph, 0 };
        ImVec2 gSize = ImGui::CalcTextSize(glyphStr);
        ImVec2 gPos(leftButtonPos.x + (buttonSize.x - gSize.x) * 0.5f,
            leftButtonPos.y + (buttonSize.y - gSize.y) * 0.5f - 0.5f);
        dl->AddText(gPos, glyphColorLeft, glyphStr);

        ImVec2 checkStart(rightButtonPos.x + buttonSize.x * 0.28f, rightButtonPos.y + buttonSize.y * 0.55f);
        ImVec2 checkMid(rightButtonPos.x + buttonSize.x * 0.42f, rightButtonPos.y + buttonSize.y * 0.70f);
        ImVec2 checkEnd(rightButtonPos.x + buttonSize.x * 0.72f, rightButtonPos.y + buttonSize.y * 0.30f);
        ImU32 checkColor = rightHeld ? IM_COL32(255, 255, 255, 255)
            : rightHovered ? IM_COL32(238, 238, 246, 255) : IM_COL32(216, 216, 226, 255);
        dl->AddLine(checkStart, checkMid, checkColor, 2.f);
        dl->AddLine(checkMid, checkEnd, checkColor, 2.f);

        splitter.Merge(dl);
        ImGui::PopID();

        ImVec2 nextPos(origin.x, origin.y + rowHeight);
        ImGui::SetCursorScreenPos(origin);
        ImGui::Dummy(ImVec2(innerWidth, rowHeight));
        return rowHeight;
    }

    static float ProfilePathRowSimple(const char* id, const char* label, char* buffer, size_t bufferSize,
        char leftGlyph, bool& leftTriggered, const ImVec2& origin, float innerWidth)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImDrawListSplitter splitter;
        splitter.Split(dl, 2);
        splitter.SetCurrentChannel(dl, 1);

        leftTriggered = false;

        const float fontSize = ImGui::GetFontSize();
        const float fieldHeight = fontSize + 10.f;
        const float rowHeight = fieldHeight + 8.f;

        float fieldWidth = ImClamp(innerWidth * 0.64f, 220.f, innerWidth - 20.f);
        float fieldX = origin.x + innerWidth - fieldWidth;
        if (fieldX < origin.x + 90.f)
            fieldX = origin.x + 90.f;
        float fieldY = origin.y + (rowHeight - fieldHeight) * 0.5f;

        const float buttonWidth = 32.f;
        const float buttonGap = 0.f;
        float inputWidth = ImMax(fieldWidth - buttonWidth - buttonGap, 80.f);

        ImGui::SetCursorScreenPos(origin);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.64f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImGui::PushID(id);

        ImVec2 leftButtonPos(fieldX, fieldY);
        ImVec2 buttonSize(buttonWidth, fieldHeight);
        ImGui::SetCursorScreenPos(leftButtonPos);
        ImGui::InvisibleButton("##left_button", buttonSize);
        bool leftHovered = ImGui::IsItemHovered();
        bool leftHeld = ImGui::IsItemActive();
        leftTriggered = ImGui::IsItemClicked();

        ImVec2 inputPos(fieldX + buttonWidth + buttonGap, fieldY);
        ImVec2 inputSize(inputWidth, fieldHeight);
        ImGui::SetCursorScreenPos(inputPos);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.f, (fieldHeight - fontSize) * 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushItemWidth(inputSize.x);
        ImGui::InputText("##profile_path_simple", buffer, bufferSize);
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);

        bool inputHovered = ImGui::IsItemHovered();
        bool inputActive = ImGui::IsItemActive();

        if (leftHovered)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        bool anyHovered = inputHovered || leftHovered;
        bool anyHeld = inputActive || leftHeld;

        ImVec4 baseFill = ImLerp(g_guiColorFrame, ImVec4(0.f, 0.f, 0.f, g_guiColorFrame.w), 0.25f);
        ImVec4 hoverFill = ImLerp(baseFill, g_guiColorAccent, 0.25f);
        ImVec4 activeFill = ImVec4(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 0.65f);

        ImVec4 fill = baseFill;
        if (anyHovered)
            fill = ImLerp(fill, hoverFill, 0.8f);
        if (anyHeld)
            fill = ImLerp(fill, activeFill, 0.65f);

        ImVec4 borderBase = ImLerp(g_guiColorFrame, g_guiColorAccent, 0.2f);
        ImVec4 borderAccent = ImVec4(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 0.85f);
        ImVec4 borderCol = anyHeld ? ImLerp(borderBase, borderAccent, 0.6f)
            : anyHovered ? ImLerp(borderBase, borderAccent, 0.3f) : borderBase;

        ImVec2 combinedMin(leftButtonPos);
        ImVec2 combinedMax(inputPos.x + inputSize.x, inputPos.y + inputSize.y);
        float rounding = fieldHeight * 0.5f;

        splitter.SetCurrentChannel(dl, 0);
        dl->AddRectFilled(combinedMin, combinedMax, ImGui::ColorConvertFloat4ToU32(fill), rounding);
        dl->AddRect(combinedMin, combinedMax, ImGui::ColorConvertFloat4ToU32(borderCol), rounding, 0, 1.1f);

        ImVec2 leftSepMin(leftButtonPos.x + buttonSize.x, leftButtonPos.y + 6.f);
        ImVec2 leftSepMax(leftSepMin.x, leftButtonPos.y + buttonSize.y - 6.f);
        dl->AddLine(leftSepMin, leftSepMax, IM_COL32(86, 86, 100, 210), 1.1f);

        ImU32 glyphColorLeft = leftHeld ? IM_COL32(255, 255, 255, 255)
            : leftHovered ? IM_COL32(238, 238, 246, 255) : IM_COL32(216, 216, 226, 255);
        char glyphStr[2] = { leftGlyph, 0 };
        ImVec2 gSize = ImGui::CalcTextSize(glyphStr);
        ImVec2 gPos(leftButtonPos.x + (buttonSize.x - gSize.x) * 0.5f,
            leftButtonPos.y + (buttonSize.y - gSize.y) * 0.5f - 0.5f);
        dl->AddText(gPos, glyphColorLeft, glyphStr);

        splitter.Merge(dl);
        ImGui::PopID();

        ImVec2 nextPos(origin.x, origin.y + rowHeight);
        ImGui::SetCursorScreenPos(origin);
        ImGui::Dummy(ImVec2(innerWidth, rowHeight));
        return rowHeight;
    }
    static float TextureInputRow(const char* id, const char* label, TextureTargetEntry& entry,
        bool& applyTriggered, bool& resetTriggered, const ImVec2& origin, float innerWidth)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImDrawListSplitter splitter;
        splitter.Split(dl, 2);
        splitter.SetCurrentChannel(dl, 1);

        applyTriggered = false;
        resetTriggered = false;

        const float fontSize = ImGui::GetFontSize();
        const float fieldHeight = fontSize + 10.f;
        const float rowHeight = fieldHeight + 8.f;

        float fieldWidth = ImClamp(innerWidth * 0.64f, 220.f, innerWidth - 20.f);
        float fieldX = origin.x + innerWidth - fieldWidth;
        if (fieldX < origin.x + 90.f)
            fieldX = origin.x + 90.f;
        float fieldY = origin.y + (rowHeight - fieldHeight) * 0.5f;

        const float buttonWidth = 32.f;
        const float buttonGap = 0.f;
        float inputWidth = ImMax(fieldWidth - (buttonWidth * 2.f) - (buttonGap * 2.f), 80.f);

        ImGui::SetCursorScreenPos(origin);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.64f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImGui::PushID(id);

        ImVec2 leftButtonPos(fieldX, fieldY);
        ImVec2 buttonSize(buttonWidth, fieldHeight);
        ImGui::SetCursorScreenPos(leftButtonPos);
        ImGui::InvisibleButton("##default_button", buttonSize);
        bool leftHovered = ImGui::IsItemHovered();
        bool leftHeld = ImGui::IsItemActive();
        resetTriggered = ImGui::IsItemClicked();

        ImVec2 inputPos(fieldX + buttonWidth + buttonGap, fieldY);
        ImVec2 inputSize(inputWidth, fieldHeight);
        ImGui::SetCursorScreenPos(inputPos);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.f, (fieldHeight - fontSize) * 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushItemWidth(inputSize.x);
        ImGui::InputText("##texture_input", entry.path.data(), entry.path.size());
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);

        bool inputHovered = ImGui::IsItemHovered();
        bool inputActive = ImGui::IsItemActive();

        ImVec2 rightButtonPos(inputPos.x + inputSize.x + buttonGap, fieldY);
        ImGui::SetCursorScreenPos(rightButtonPos);
        ImGui::InvisibleButton("##apply_button", buttonSize);
        bool rightHovered = ImGui::IsItemHovered();
        bool rightHeld = ImGui::IsItemActive();
        applyTriggered = ImGui::IsItemClicked();

        if (leftHovered || rightHovered)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        bool anyHovered = inputHovered || leftHovered || rightHovered;
        bool anyHeld = inputActive || leftHeld || rightHeld;

        ImVec4 baseFill = ImLerp(g_guiColorFrame, ImVec4(0.f, 0.f, 0.f, g_guiColorFrame.w), 0.25f);
        ImVec4 hoverFill = ImLerp(baseFill, g_guiColorAccent, 0.25f);
        ImVec4 activeFill = ImVec4(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 0.65f);

        ImVec4 fill = baseFill;
        if (anyHovered)
            fill = ImLerp(fill, hoverFill, 0.8f);
        if (anyHeld)
            fill = ImLerp(fill, activeFill, 0.65f);

        ImVec4 borderBase = ImLerp(g_guiColorFrame, g_guiColorAccent, 0.2f);
        ImVec4 borderAccent = ImVec4(g_guiColorAccent.x, g_guiColorAccent.y, g_guiColorAccent.z, 0.85f);
        ImVec4 borderCol = anyHeld ? ImLerp(borderBase, borderAccent, 0.6f)
            : anyHovered ? ImLerp(borderBase, borderAccent, 0.3f) : borderBase;

        ImVec2 combinedMin(leftButtonPos);
        ImVec2 combinedMax(rightButtonPos.x + buttonSize.x, rightButtonPos.y + buttonSize.y);
        float rounding = fieldHeight * 0.5f;

        splitter.SetCurrentChannel(dl, 0);
        dl->AddRectFilled(combinedMin, combinedMax, ImGui::ColorConvertFloat4ToU32(fill), rounding);
        dl->AddRect(combinedMin, combinedMax, ImGui::ColorConvertFloat4ToU32(borderCol), rounding, 0, 1.1f);

        ImVec2 leftSepMin(leftButtonPos.x + buttonSize.x, leftButtonPos.y + 6.f);
        ImVec2 leftSepMax(leftSepMin.x, leftButtonPos.y + buttonSize.y - 6.f);
        ImVec2 rightSepMin(rightButtonPos.x, rightButtonPos.y + 6.f);
        ImVec2 rightSepMax(rightSepMin.x, rightButtonPos.y + buttonSize.y - 6.f);
        dl->AddLine(leftSepMin, leftSepMax, IM_COL32(86, 86, 100, 210), 1.1f);
        dl->AddLine(rightSepMin, rightSepMax, IM_COL32(86, 86, 100, 210), 1.1f);

        ImU32 glyphColorLeft = leftHeld ? IM_COL32(255, 255, 255, 255)
            : leftHovered ? IM_COL32(238, 238, 246, 255) : IM_COL32(216, 216, 226, 255);
        ImVec2 dSize = ImGui::CalcTextSize(XS("D"));
        ImVec2 dPos(leftButtonPos.x + (buttonSize.x - dSize.x) * 0.5f,
            leftButtonPos.y + (buttonSize.y - dSize.y) * 0.5f - 0.5f);
        dl->AddText(dPos, glyphColorLeft, XS("D"));

        ImVec2 checkStart(rightButtonPos.x + buttonSize.x * 0.28f, rightButtonPos.y + buttonSize.y * 0.55f);
        ImVec2 checkMid(rightButtonPos.x + buttonSize.x * 0.42f, rightButtonPos.y + buttonSize.y * 0.70f);
        ImVec2 checkEnd(rightButtonPos.x + buttonSize.x * 0.72f, rightButtonPos.y + buttonSize.y * 0.30f);
        ImU32 checkColor = rightHeld ? IM_COL32(255, 255, 255, 255)
            : rightHovered ? IM_COL32(238, 238, 246, 255) : IM_COL32(216, 216, 226, 255);
        dl->AddLine(checkStart, checkMid, checkColor, 2.f);
        dl->AddLine(checkMid, checkEnd, checkColor, 2.f);

        splitter.Merge(dl);
        ImGui::PopID();

        ImVec2 nextPos(origin.x, origin.y + rowHeight);
        ImGui::SetCursorScreenPos(origin);
        ImGui::Dummy(ImVec2(innerWidth, rowHeight));
        return rowHeight;
    }

}

float GuiToggleTextRow(const char* id, const char* label, const char* valueText, bool& state,
    const ImVec2& origin, float innerWidth)
{
    const bool bindHoldRow = (g_currentGuiFeature != nullptr) && IsBindHoldRow(label, valueText);
    if (bindHoldRow)
        state = g_currentGuiFeature->KeybindHold();
    else
        BindBoolFieldIfSafe(id, &state);

    float row = ToggleTextRow(id, label, valueText, state, origin, innerWidth);

    if (bindHoldRow)
        g_currentGuiFeature->SetKeybindHold(state);

    return row;
}
float GuiEnabledDisabledRow(const char* id, const char* label, bool& state,
    const ImVec2& origin, float innerWidth)
{
    BindBoolFieldIfSafe(id, &state);
    return ToggleTextRow(id, label, XS("Enabled"), state, origin, innerWidth);
}
float GuiFeatureEnabledRow(const char* id, const char* label, Feature& feature,
    const ImVec2& origin, float innerWidth)
{
    bool state = feature.Enabled();
    float row = ToggleTextRow(id, label, XS("Enabled"), state, origin, innerWidth);
    if (state != feature.Enabled())
        feature.SetEnabled(state);
    return row;
}
float GuiRangeSliderRow(const char* id, const char* label, float* v_min, float* v_max, float v_lower, float v_upper,
    const char* display_fmt, const ImVec2& origin, float innerWidth, bool brightLabel)
{
    if (id && id[0])
    {
        std::string keyMin = std::string(id) + ".min";
        std::string keyMax = std::string(id) + ".max";
        BindFloatFieldIfSafe(keyMin.c_str(), v_min);
        BindFloatFieldIfSafe(keyMax.c_str(), v_max);
    }
    return RangeSliderRow(id, label, v_min, v_max, v_lower, v_upper, display_fmt, origin, innerWidth, brightLabel);
}
float GuiDualListInputRow(const char* id,
    const char* leftLabel, const char* rightLabel,
    char* leftBuffer, size_t leftBufferSize,
    char* rightBuffer, size_t rightBufferSize,
    bool& leftAddTriggered, bool& rightAddTriggered,
    const ImVec2& origin, float innerWidth)
{
    if (id && id[0])
    {
        std::string leftKey = std::string(id) + ".left";
        std::string rightKey = std::string(id) + ".right";
        BindStringFieldIfSafe(leftKey.c_str(), leftBuffer, leftBufferSize);
        BindStringFieldIfSafe(rightKey.c_str(), rightBuffer, rightBufferSize);
    }

    return DualListInputRow(id, leftLabel, rightLabel, leftBuffer, leftBufferSize,
        rightBuffer, rightBufferSize, leftAddTriggered, rightAddTriggered, origin, innerWidth);
}
float GuiInputAddRow(const char* id, const char* label, char* buffer, size_t bufferSize,
    bool& addTriggered, const ImVec2& origin, float innerWidth)
{
    BindStringFieldIfSafe(id, buffer, bufferSize);
    return InputAddRow(id, label, buffer, bufferSize, addTriggered, origin, innerWidth);
}
float GuiInputConfirmRow(const char* id, const char* label, char* buffer, size_t bufferSize,
    bool& confirmed, const ImVec2& origin, float innerWidth)
{
    BindStringFieldIfSafe(id, buffer, bufferSize);
    return InputConfirmRow(id, label, buffer, bufferSize, confirmed, origin, innerWidth);
}
float GuiInputActionConfirmRow(const char* id, const char* label, char* buffer, size_t bufferSize,
    char leftGlyph, bool& leftTriggered, bool& confirmed, const ImVec2& origin, float innerWidth)
{
    BindStringFieldIfSafe(id, buffer, bufferSize);
    return ProfilePathRow(id, label, buffer, bufferSize, leftGlyph, leftTriggered, confirmed, origin, innerWidth);
}
float GuiDropdownRow(const char* id, const char* label, const char* const* options, int optionCount,
    int& currentIndex, const ImVec2& origin, float innerWidth)
{
    BindIntFieldIfSafe(id, &currentIndex);
    return DropdownRow(id, label, options, optionCount, currentIndex, origin, innerWidth);
}
float GuiSliderRow(const char* id, const char* label, float* value, float v_min, float v_max, const char* display_fmt,
    const ImVec2& origin, float innerWidth, bool brightLabel)
{
    BindFloatFieldIfSafe(id, value);
    return SliderRow(id, label, value, v_min, v_max, display_fmt, origin, innerWidth, brightLabel);
}
float GuiCapsuleListRow(const char* id, std::vector<CapsuleEntry>& entries,
    const ImVec2& origin, float innerWidth)
{
    return CapsuleListRow(id, entries, origin, innerWidth, nullptr, true, false);
}
float GuiCapsuleListRowEx(const char* id, std::vector<CapsuleEntry>& entries,
    const ImVec2& origin, float innerWidth, int* clickedIndex, bool allowContextEdit, bool edgeCloseStyle)
{
    return CapsuleListRow(id, entries, origin, innerWidth, clickedIndex, allowContextEdit, edgeCloseStyle);
}
float GuiColorButtonRow(const char* id, const char* label, ImVec4& color,
    const ImVec2& origin, float innerWidth, bool enablePicker)
{
    BindColorFieldIfSafe(id, &color);
    return ColorButtonRow(id, label, color, origin, innerWidth, enablePicker);
}
float GuiTextureInputRow(const char* id, const char* label, TextureTargetEntry& entry,
    bool& applyTriggered, bool& resetTriggered, const ImVec2& origin, float innerWidth)
{
    BindStringFieldIfSafe(id, entry.path.data(), entry.path.size());
    return TextureInputRow(id, label, entry, applyTriggered, resetTriggered, origin, innerWidth);
}
float GuiDetailRowText(const char* label, const char* value, const ImVec2& origin, float innerWidth, bool drawSeparator)
{
    return DetailRowText(label, value, origin, innerWidth, drawSeparator);
}
void GuiCustomCheckbox(const char* id, const char* label, bool& value, const ImVec2& pos)
{
    BindBoolFieldIfSafe(id, &value);
    CustomCheckbox(id, label, value, pos);
}
void GuiAddCapsuleTokens(const std::string& raw, std::vector<CapsuleEntry>& target, const ImVec4& defaultColor)
{
    AddCapsuleTokens(raw, target, defaultColor);
}
void GuiSetNextHeaderSplitBadge(const char* cardId, int* value, const char* leftLabel, const char* rightLabel, bool* linkedBool)
{
    SetNextHeaderSplitBadge(cardId, value, leftLabel, rightLabel, linkedBool);
}
void GuiBindConfigBool(const char* id, bool* value)
{
    BindBoolFieldIfSafe(id, value);
}
void GuiBindConfigInt(const char* id, int* value)
{
    BindIntFieldIfSafe(id, value);
}
void GuiBindConfigFloat(const char* id, float* value)
{
    BindFloatFieldIfSafe(id, value);
}
void GuiBindConfigString(const char* id, char* value, size_t size)
{
    BindStringFieldIfSafe(id, value, size);
}
void GuiBindConfigColor(const char* id, ImVec4* value)
{
    BindColorFieldIfSafe(id, value);
}

ImVec4& GuiColorWindow() { return g_guiColorWindow; }
ImVec4& GuiColorChild() { return g_guiColorChild; }
ImVec4& GuiColorFrame() { return g_guiColorFrame; }
ImVec4& GuiColorButton() { return g_guiColorButton; }
ImVec4& GuiColorText() { return g_guiColorText; }
ImVec4& GuiColorTextEnabled() { return g_guiColorTextEnabled; }
ImVec4& GuiColorTextDisabled() { return g_guiColorTextDisabled; }
ImVec4& GuiColorAccent() { return g_guiColorAccent; }
ImVec4& GuiColorOutline() { return g_guiColorOutline; }
ImVec4& GuiColorHighlight() { return g_guiColorHighlight; }
ImVec4& GuiColorBadge() { return g_guiColorBadge; }
ImVec4& GuiOverlayColor() { return g_guiOverlayColor; }
const ImVec4& GuiActiveAccent() { return g_activeAccent; }
void GuiApplyStyle() { ApplyGuiStyle(); }
void GuiResetColors()
{
    ResetGuiColorsInternal();
    ApplyGuiStyle();
}
static char profileUID[32]{};
static char profileLevel[16]{};
static char profileWorldLevel[16]{};
static char profileCurExp[32]{};
static char profileMaxExp[32]{};
static char profileInfo[32]{};
static char profileExpBar[32]{};
static char profileSign[32]{};
static char profileNoSign[32]{};
static char profileUIDd[32]{};
static char profileBirthday[32]{};
static char profileIconPath[260]{};
static char profileBgPath[260]{};
static int profileModeIndex = 0;
static bool profileMenuVisible = false;
static bool profileOverlayExpanded = false;
static bool profileTextRestoreQueued = false;
static ImVec4 profileColorAll = ImVec4(1.f, 1.f, 1.f, 1.f);
static ImVec4 profileColorName = ImVec4(1.f, 1.f, 1.f, 1.f);
static ImVec4 profileColorLevel = ImVec4(1.f, 1.f, 1.f, 1.f);
static ImVec4 profileColorUid = ImVec4(1.f, 1.f, 1.f, 1.f);
static ImVec4 profileColorExp = ImVec4(1.f, 1.f, 1.f, 1.f);


void GuiRender()
{
    if (!g_showSettings) return;
       
    


        ImGuiIO& io = ImGui::GetIO();
       /* TickWaypointMessageTimer();
        auto& waypointUi = GetWaypointUiState();
        bool waypointEditor = waypointUi.windowOpen;
        ApplyGuiColors();*/
        ImDrawList* overlay = ImGui::GetBackgroundDrawList();
        overlay->AddRectFilled(ImVec2(0.f, 0.f), io.DisplaySize,
            ImGui::ColorConvertFloat4ToU32(g_guiOverlayColor));

        ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.f));

        const ImGuiWindowFlags rootFlags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse;

        if (fabsf(io.MouseWheel) > FLT_EPSILON)
        {
            g_menuScroll -= io.MouseWheel * 60.f;
            if (g_menuScroll < 0.f) g_menuScroll = 0.f;
        }

        if (ImGui::Begin(XS("menu_root"), &g_showSettings, rootFlags))
        {
            float maxWidth = ImMax(io.DisplaySize.x - 40.f, 740.f);
            const float panelWidth = ImClamp(io.DisplaySize.x * 0.66f, 780.f, maxWidth);
            const float panelX = (io.DisplaySize.x - panelWidth) * 0.5f;
            const float basePanelY = ImMax(18.f, io.DisplaySize.y * 0.04f);
            const float panelY = basePanelY - g_menuScroll;

            ImVec2 panelPos(panelX, panelY);
            ImGui::SetCursorScreenPos(panelPos);

            ImDrawList* dl = ImGui::GetWindowDrawList();
            const float capsuleHeight = 48.f;

            ImVec2 capsuleMin = panelPos;
            ImVec2 capsuleMax = add(capsuleMin,ImVec2(panelWidth, capsuleHeight));
            ImVec4 topFill = ImLerp(g_guiColorFrame, ImVec4(0.f, 0.f, 0.f, g_guiColorFrame.w), 0.35f);
            topFill.w = ImClamp(topFill.w, 0.f, 1.f);
            dl->AddRectFilled(capsuleMin, capsuleMax, ImGui::ColorConvertFloat4ToU32(topFill), capsuleHeight * 0.5f);

            float fontSize = ImGui::GetFontSize();

            float tabHeight = fontSize + 16.f;
            float tabYOffset = ImMax((capsuleHeight - tabHeight) * 0.5f, 0.f);
            static int tab = 0;
            static std::string tabStr0, tabStr1, tabStr2, tabStr3, tabStr4;
            tabStr0 = XS("Combat");  tabStr1 = XS("Move");  tabStr2 = XS("Visual");
            tabStr3 = XS("Misc");    tabStr4 = XS("Profiles");
            const char* tabNames[5] = { tabStr0.c_str(), tabStr1.c_str(), tabStr2.c_str(), tabStr3.c_str(), tabStr4.c_str() };

            float tabGap = 10.f;
            float itemWidth = (panelWidth - 32.f - tabGap * 4.f) / 5.f;
            float totalTabsWidth = itemWidth * 5.f + tabGap * 4.f;
            float tabStartX = capsuleMin.x + ImMax((panelWidth - totalTabsWidth) * 0.5f, 16.f);
            //if (!waypointEditor)
            {
                ImGui::SetCursorScreenPos(ImVec2(tabStartX, capsuleMin.y + tabYOffset));
                ImGui::PushID("capsule_tabs");
                for (int i = 0; i < 5; ++i)
                {
                    if (i) ImGui::SameLine(0.f, tabGap);
                    if (CapsuleTab(tabNames[i], tab == i, itemWidth))
                        tab = i;
                }
                ImGui::PopID();
            }
            /*else
            {
                ImGui::SetCursorScreenPos(ImVec2(capsuleMin.x + 28.f, capsuleMin.y + (capsuleHeight - fontSize) * 0.5f - 1.f));
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.86f));
                ImGui::TextUnformatted(XS("Waypoints"));
                ImGui::PopStyleColor();
            }*/

            g_activeAccent = AccentForTab(tab);

            const float columnGap = 24.f;
            const float cardWidth = (panelWidth - columnGap) * 0.5f;
            const float innerWidth = cardWidth - 48.f;
            const float cardSpacing = 12.f;

            const float leftColumnX = panelPos.x;
            const float rightColumnX = panelPos.x + cardWidth + columnGap;
            float leftY = capsuleMax.y + cardSpacing;
            float rightY = capsuleMax.y + cardSpacing;
            Feature* pendingCaptureFeature = nullptr;

            auto renderCard = [&](Feature* owner, const char* id, const char* title, const char* desc, const char* badge,
                CardState& state, float columnX, float baseY,
                const std::function<float(const ImVec2&, float)>& contentFn,
                bool allowCustomBadgeClick = false, bool* outCustomBadgeClicked = nullptr) -> float
                {
                    if (outCustomBadgeClicked)
                        *outCustomBadgeClicked = false;
                    float dt = io.DeltaTime;
                    float target = state.expanded ? 1.f : 0.f;
                    float speed = state.expanded ? 18.f : 14.f;
                    state.anim += (target - state.anim) * ImClamp(dt * speed, 0.f, 1.f);
                    state.anim = ImSaturate(state.anim);
                    float eased = state.anim * state.anim * (3.f - 2.f * state.anim);

                    ImVec2 cardPos(columnX, baseY);
                    ImGui::SetCursorScreenPos(cardPos);

                    ImDrawList* dlCard = ImGui::GetWindowDrawList();
                    dlCard->ChannelsSplit(2);
                    dlCard->ChannelsSetCurrent(1);

                    ImVec2 cardMin;
                    float headerH;
                    ImGuiWindow* currentWindow = ImGui::GetCurrentWindow();
                    ImGuiID headerId = currentWindow->GetID(id);
                    if (g_pendingSplitBadge.cardId && std::strcmp(g_pendingSplitBadge.cardId, id) == 0)
                    {
                        HeaderSplitConfig cfg;
                        cfg.value = g_pendingSplitBadge.value;
                        cfg.linkedBool = g_pendingSplitBadge.linkedBool;
                        cfg.leftLabel = g_pendingSplitBadge.leftLabel ? g_pendingSplitBadge.leftLabel : "<";
                        cfg.rightLabel = g_pendingSplitBadge.rightLabel ? g_pendingSplitBadge.rightLabel : ">";
                        g_headerSplitConfigs[headerId] = cfg;
                        g_pendingSplitBadge = {};
                    }

                    bool badgeClicked = false;
                    const bool allowFeatureBadgeCapture = owner && ShouldUseFeatureKeybindBadge(*owner, badge);
                    const bool allowBadgeCapture = allowFeatureBadgeCapture || allowCustomBadgeClick;
                    FunctionHeader(id, title, desc, badge, state, cardWidth, eased,
                        allowBadgeCapture, &badgeClicked, &cardMin, &headerH);
                    if (outCustomBadgeClicked && allowCustomBadgeClick)
                        *outCustomBadgeClicked = badgeClicked;
                    if (badgeClicked && owner && allowFeatureBadgeCapture)
                        pendingCaptureFeature = owner;
                    ImVec2 detailMin(cardMin.x, cardMin.y + headerH);

                    float curHeight = state.detailHeight * eased;
                    if (state.expanded || eased > 0.001f)
                    {
                        ImGui::SetCursorScreenPos(detailMin);
                        ImGui::PushClipRect(detailMin, add(detailMin,ImVec2(cardWidth, ImMax(curHeight, 1.f))), true);
                        Feature* prevGuiFeature = g_currentGuiFeature;
                        g_currentGuiFeature = owner;
                        float measured = contentFn(detailMin, innerWidth);
                        g_currentGuiFeature = prevGuiFeature;
                        ImGui::PopClipRect();
                        if (measured > 0.f)
                            state.detailHeight = ImLerp(state.detailHeight, measured, 0.35f);
                    }

                    dlCard->ChannelsSetCurrent(0);

                    float inflate = state.hover * 3.0f;
                    ImVec2 bgMin(cardMin.x - inflate, cardMin.y - inflate * 0.4f);
                    ImVec2 bgMax(cardMin.x + cardWidth + inflate, detailMin.y + curHeight + inflate * 0.6f);
                    float radius = 20.f;

                    ImVec4 baseFill = ImLerp(g_guiColorChild, ImVec4(0.f, 0.f, 0.f, g_guiColorChild.w), 0.18f);
                    ImVec4 hoverFill = ImLerp(baseFill, g_guiColorFrame, 0.35f);
                    ImVec4 fillCol = ImLerp(baseFill, hoverFill, state.hover);
                    dlCard->AddRectFilled(bgMin, bgMax,
                        ImGui::ColorConvertFloat4ToU32(fillCol), radius);

                    float glowAmount = ImSaturate(state.pressGlow);
                    if (glowAmount > 0.001f)
                    {
                        ImVec4 overlay = g_guiColorHighlight;
                        overlay.w = ImClamp(overlay.w * glowAmount, 0.f, 1.f);
                        dlCard->AddRectFilled(bgMin, bgMax,
                            ImGui::ColorConvertFloat4ToU32(overlay), radius);
                    }

                    ImVec4 borderBase = g_guiColorOutline;
                    ImVec4 borderHighlight = ImLerp(borderBase,
                        ImVec4(g_guiColorHighlight.x, g_guiColorHighlight.y, g_guiColorHighlight.z,
                            ImClamp(g_guiColorHighlight.w + 0.35f, 0.f, 1.f)),
                        0.65f);
                    float borderMix = ImSaturate(state.pressGlow + state.hover * 0.45f);
                    ImVec4 borderCol = ImLerp(borderBase, borderHighlight, borderMix);
                    dlCard->AddRect(bgMin, bgMax,
                        ImGui::ColorConvertFloat4ToU32(borderCol), radius, 0, 1.35f);

                    float accentAlpha = ImSaturate(state.pressGlow * 0.6f + state.hover * 0.25f);
                    ImVec4 accentOutline = g_guiColorHighlight;
                    accentOutline.w = ImClamp(accentOutline.w * (accentAlpha * 0.85f + 0.05f), 0.f, 1.f);
                    dlCard->AddRect(add(bgMin,ImVec2(1.4f, 1.4f)), sub(bgMax,ImVec2(1.4f, 1.4f)),
                        ImGui::ColorConvertFloat4ToU32(accentOutline),
                        radius - 1.4f, 0, 1.0f);

                    float sepAlpha = ImSaturate(eased);
                    if (sepAlpha > 0.05f && curHeight > 6.f)
                    {
                        float halfWidth = ImClamp(cardWidth * 0.35f, 32.f, (cardWidth * 0.5f) - 32.f);
                        float sepY = detailMin.y + 5.f;
                        ImVec2 sepStart(detailMin.x + cardWidth * 0.5f - halfWidth, sepY);
                        ImVec2 sepEnd(detailMin.x + cardWidth * 0.5f + halfWidth, sepY);
                        dlCard->AddLine(sepStart, sepEnd, IM_COL32(122, 122, 132, (int)(140 * sepAlpha)), 1.1f);
                    }

                    dlCard->ChannelsMerge();

                    float totalHeight = headerH + curHeight;
                    ImGui::SetCursorScreenPos(ImVec2(cardPos.x, cardPos.y + totalHeight));
                    ImGui::Dummy(ImVec2(cardWidth, 0.01f));
                    return cardPos.y + totalHeight + cardSpacing;
                };

            float bottom = 0.f;
            auto renderFeaturesForTab = [&](FeatureTab currentTab)
                {
                    auto& features = GetFeatureManager().All();
                    for (auto& entry : features)
                    {
                        Feature* feature = entry.get();
                        if (!feature || feature->Info().tab != currentTab)
                            continue;

                        float* colY = (leftY <= rightY) ? &leftY : &rightY;
                        float colX = (colY == &leftY) ? leftColumnX : rightColumnX;

                        feature->card.active = feature->Enabled();
                        bool wasActive = feature->card.active;
                        if (const HeaderSplitBadge* badge = feature->HeaderSplitBadgeConfig())
                            GuiSetNextHeaderSplitBadge(feature->Info().id, badge->value, badge->leftLabel, badge->rightLabel, badge->linkedBool);
                        std::string badgeText = BuildFeatureBadgeText(*feature, feature->Info().badge);
                        *colY = renderCard(feature, feature->Info().id, feature->Info().title, feature->Info().desc,
                            badgeText.c_str(), feature->card, colX, *colY,
                            [feature](const ImVec2& detailStart, float width) -> float
                            {
                                return feature->OnGUI(detailStart, width);
                            });

                        if (feature->card.active != wasActive)
                            feature->SetEnabled(feature->card.active);
                    }
                };
            //if (!waypointEditor)
            {
                if (tab == 0)
                    renderFeaturesForTab(FeatureTab::Combat);
                else if (tab == 1)
                    renderFeaturesForTab(FeatureTab::Move);
                else if (tab == 2)
                    renderFeaturesForTab(FeatureTab::Visual);
                else if (tab == 3)
                    renderFeaturesForTab(FeatureTab::Misc);
                else if (tab == 4)
                {
                    renderFeaturesForTab(FeatureTab::Profiles);

                    DefaultProfileFeature* profileFeature = nullptr;
                    auto& features = GetFeatureManager().All();
                    for (auto& entry : features)
                    {
                        if (!entry)
                            continue;
                        profileFeature = dynamic_cast<DefaultProfileFeature*>(entry.get());
                        if (profileFeature)
                            break;
                    }

                    if (profileFeature)
                    {
                        profileFeature->TickStatus(io.DeltaTime);
                        const auto& cfgNames = profileFeature->ProfileNames();
                        std::unordered_set<std::string> keep(cfgNames.begin(), cfgNames.end());
                        for (auto it = g_profileCfgCards.begin(); it != g_profileCfgCards.end();)
                        {
                            if (keep.find(it->first) == keep.end())
                                it = g_profileCfgCards.erase(it);
                            else
                                ++it;
                        }
                        if (!g_selectedProfileCfg.empty() && keep.find(g_selectedProfileCfg) == keep.end())
                            g_selectedProfileCfg.clear();

                        std::string queuedLoadName;
                        std::string queuedDeleteName;

                        for (const std::string& cfgName : cfgNames)
                        {
                            CardState& cfgCard = g_profileCfgCards[cfgName];
                            std::string cardId = "card_profile_cfg_" + cfgName;
                            for (char& ch : cardId)
                            {
                                if (!(std::isalnum(static_cast<unsigned char>(ch)) || ch == '_'))
                                    ch = '_';
                            }

                            float* colY = (leftY <= rightY) ? &leftY : &rightY;
                            float colX = (colY == &leftY) ? leftColumnX : rightColumnX;
                            cfgCard.active = (!g_selectedProfileCfg.empty() && g_selectedProfileCfg == cfgName);
                            const bool wasActive = cfgCard.active;
                            bool badgeDeleteClicked = false;

                            *colY = renderCard(nullptr, cardId.c_str(), cfgName.c_str(), XS("Saved configuration"), XS("DEL"),
                                cfgCard, colX, *colY,
                                [&](const ImVec2& detailStart, float width) -> float
                                {
                                    float x = detailStart.x + 20.f;
                                    float y = detailStart.y + 12.f;
                                    y += GuiDetailRowText(XS("File"), (cfgName + XS(".cfg")).c_str(), ImVec2(x, y), width, false);
                                    y += 4.f;
                                    return (y - detailStart.y) + 8.f;
                                }, true, &badgeDeleteClicked);

                            if (badgeDeleteClicked)
                            {
                                queuedDeleteName = cfgName;
                                continue;
                            }

                            if (cfgCard.active != wasActive)
                            {
                                if (cfgCard.active)
                                {
                                    g_selectedProfileCfg = cfgName;
                                    queuedLoadName = cfgName;
                                }
                                else
                                {
                                 
                                    cfgCard.active = true;
                                }
                            }
                        }

                        if (!g_selectedProfileCfg.empty())
                        {
                            for (auto& kv : g_profileCfgCards)
                                kv.second.active = (kv.first == g_selectedProfileCfg);
                        }

                        if (!queuedDeleteName.empty())
                        {
                            std::string status;
                            profileFeature->DeleteProfile(queuedDeleteName, status);
                            profileFeature->ShowStatus(status, 3.0f);
                            if (g_selectedProfileCfg == queuedDeleteName)
                                g_selectedProfileCfg.clear();
                            profileFeature->RefreshProfiles();
                        }
                        else if (!queuedLoadName.empty())
                        {
                            std::string status;
                            profileFeature->LoadProfile(queuedLoadName, status);
                            profileFeature->SetProfileNameBuffer(queuedLoadName);
                            profileFeature->ShowStatus(status, 3.0f);
                        }
                    }
                }

                bottom = ImMax(leftY, rightY);
            }

            auto& allFeatures = GetFeatureManager().All();
            if (pendingCaptureFeature)
            {
                const bool nextCapture = !pendingCaptureFeature->KeybindCapturing();
                for (auto& entry : allFeatures)
                    entry->SetKeybindCapturing(false);
                pendingCaptureFeature->SetKeybindCapturing(nextCapture);
            }

            Feature* capturingFeature = nullptr;
            for (auto& entry : allFeatures)
            {
                if (entry->KeybindCapturing())
                {
                    capturingFeature = entry.get();
                    break;
                }
            }
            if (capturingFeature)
            {
                const int capturedVk = PollPressedVirtualKeyForCapture();
                if (capturedVk != 0)
                {
                    if (capturedVk == VK_BACK)
                        capturingFeature->ClearKeybind();
                    else if (capturedVk != VK_ESCAPE)
                    {
                        capturingFeature->SetKeybindKey(capturedVk, true);
                      
                        if (!capturingFeature->Enabled())
                            capturingFeature->SetEnabled(true);
                    }
                    capturingFeature->SetKeybindCapturing(false);
                }
            }
            //else
            //{
            //    auto& entries = GetWaypointEntries();
            //    auto& uiState = GetWaypointUiState();
            //    ImVec4 baseAccent = g_activeAccent;

            //    if (uiState.messageTimer > 0.f && !uiState.messageText.empty())
            //    {
            //        ImGui::SetCursorScreenPos(ImVec2(leftColumnX, leftY));
            //        ImGui::PushStyleColor(ImGuiCol_Text, uiState.messageColor);
            //        ImGui::PushTextWrapPos(rightColumnX + cardWidth);
            //        ImGui::TextWrapped("%s", uiState.messageText.c_str());
            //        ImGui::PopTextWrapPos();
            //        ImGui::PopStyleColor();
            //        float msgAdvance = ImGui::GetTextLineHeightWithSpacing() + 8.f;
            //        leftY += msgAdvance;
            //        rightY += msgAdvance;
            //    }

            //    /*auto placeWaypointCard = [&](const char* id, const char* title, const char* desc, size_t stateIndex,
            //        const std::function<float(const ImVec2&, float)>& fn, const char* badge, const ImVec4* accentOverride)
            //        {
            //            ImVec4 previousAccent = g_activeAccent;
            //            g_activeAccent = accentOverride ? *accentOverride : baseAccent;
            //            if (leftY <= rightY)
            //                leftY = renderCard(id, title, desc, badge, WaypointCardState(stateIndex), leftColumnX, leftY, fn);
            //            else
            //                rightY = renderCard(id, title, desc, badge, WaypointCardState(stateIndex), rightColumnX, rightY, fn);
            //            g_activeAccent = previousAccent;
            //        };

            //    placeWaypointCard("card_waypoint_add", "New Waypoint", "Capture current position", 0,
            //        [&](const ImVec2& detailStart, float width) -> float
            //        {
            //            float x = detailStart.x + 20.f;
            //            float y = detailStart.y + 12.f;

            //            ImVec2 captureSize(160.f, 32.f);
            //            ImGui::SetCursorScreenPos(ImVec2(x, y));
            //            if (ImGui::Button("CAPTURE CURRENT", captureSize))
            //            {
            //                WaypointCoords coords;
            //                if (CaptureCurrentPlayerCoords(coords))
            //                {
            //                    uiState.pendingCoords = coords;
            //                    uiState.editorOpen = true;
            //                    uiState.pendingColor = ImVec4(0.32f, 0.62f, 0.98f, 1.f);
            //                    if (uiState.nameBuffer[0] == '\0')
            //                        std::snprintf(uiState.nameBuffer, IM_ARRAYSIZE(uiState.nameBuffer), "Waypoint %zu", entries.size() + 1);
            //                }
            //                else
            //                {
            //                    SetWaypointMessage("Unable to read player position.", ImVec4(0.97f, 0.48f, 0.48f, 1.f));
            //                }
            //            }
            //            y += captureSize.y + 10.f;

            //            ImVec4 coordColor = uiState.editorOpen ? ImVec4(1.f, 1.f, 1.f, 0.8f) : ImVec4(1.f, 1.f, 1.f, 0.5f);
            //            ImGui::SetCursorScreenPos(ImVec2(x, y));
            //            ImGui::PushStyleColor(ImGuiCol_Text, coordColor);
            //            Vector3 absPos = GetAbsolutePosition(currentPlayerPos);
            //            ImGui::Text("X: %.2f   Y: %.2f   Z: %.2f",
            //                absPos.x, absPos.y, absPos.z);
            //            ImGui::PopStyleColor();
            //            y += ImGui::GetTextLineHeightWithSpacing();

            //            if (uiState.editorOpen)
            //            {
            //                ImGui::SetCursorScreenPos(ImVec2(x, y));
            //                ImGui::PushItemWidth(width);
            //                ImGui::InputText("##waypoint_name", uiState.nameBuffer, IM_ARRAYSIZE(uiState.nameBuffer));
            //                ImGui::PopItemWidth();
            //                y += ImGui::GetFrameHeightWithSpacing();

            //                ImGui::SetCursorScreenPos(ImVec2(x, y));
            //                ImGui::PushItemWidth(width);
            //                ImGui::ColorEdit3("##waypoint_color", &uiState.pendingColor.x,
            //                    ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoInputs);
            //                ImGui::PopItemWidth();
            //                y += ImGui::GetFrameHeightWithSpacing();

            //                ImVec2 okSize(90.f, 30.f);
            //                ImGui::SetCursorScreenPos(ImVec2(x, y));
            //                if (ImGui::Button("SAVE", okSize))
            //                {
            //                    WaypointEntry entry{};
            //                    entry.coords = uiState.pendingCoords;
            //                    entry.color = uiState.pendingColor;
            //                    entry.name = uiState.nameBuffer[0] ? uiState.nameBuffer : std::string("Waypoint ") + std::to_string(entries.size() + 1);
            //                    entries.push_back(entry);
            //                    ResetWaypointEditorState();
            //                    SetWaypointMessage("Waypoint saved.", ImVec4(0.62f, 0.92f, 0.78f, 1.f));
            //                }
            //                ImGui::SameLine(0.f, 12.f);
            //                if (ImGui::Button("CANCEL", okSize))
            //                {
            //                    ResetWaypointEditorState();
            //                    SetWaypointMessage("Waypoint creation canceled.", ImVec4(0.95f, 0.74f, 0.28f, 1.f));
            //                }
            //                y += okSize.y + 6.f;
            //            }
            //            else
            //            {
            //                ImGui::SetCursorScreenPos(ImVec2(x, y));
            //                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.48f));
            //                ImGui::TextUnformatted("Capture current position to edit name or color.");
            //                ImGui::PopStyleColor();
            //                y += ImGui::GetTextLineHeightWithSpacing();
            //            }

            //            return (y - detailStart.y) + 12.f;
            //        },
            //        "CREATE", nullptr);

            //    int removeIndex = -1;
            //    for (size_t i = 0; i < entries.size(); ++i)
            //    {
            //        WaypointEntry& entry = entries[i];
            //        std::string cardId = "card_waypoint_entry_" + std::to_string(i);
            //        std::string title = entry.name.empty() ? std::string("Waypoint ") + std::to_string(i + 1) : entry.name;
            //        const char* badge = entry.enabled ? "ACTIVE" : "HIDDEN";
            //        ImVec4 accent = entry.color;
            //        accent.w = 0.95f;

            //        placeWaypointCard(cardId.c_str(), title.c_str(), "Teleport anchor", i + 1,
            //            [&](const ImVec2& detailStart, float width) -> float
            //            {
            //                float x = detailStart.x + 20.f;
            //                float y = detailStart.y + 12.f;

            //                ImGui::SetCursorScreenPos(ImVec2(x, y));
            //                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.65f));
            //                ImGui::Text("X: %.2f   Y: %.2f   Z: %.2f",
            //                    entry.coords.x, entry.coords.y, entry.coords.z);
            //                ImGui::PopStyleColor();
            //                y += ImGui::GetTextLineHeightWithSpacing();

            //                ImVec2 warpSize(110.f, 30.f);
            //                ImVec2 copySize(90.f, 30.f);
            //                ImGui::SetCursorScreenPos(ImVec2(x, y));
            //                bool warpDisabled = !entry.enabled;
            //                if (warpDisabled) ImGui::BeginDisabled();

            //                std::string warpBtnId = "WARP##" + std::to_string(i);
            //                if (ImGui::Button(warpBtnId.c_str(), warpSize))
            //                {
            //                    if (TeleportToWaypoint(entry.coords))
            //                    {
            //                        std::string msg = std::string("Warped to ") + (entry.name.empty() ? "waypoint" : entry.name);
            //                        SetWaypointMessage(msg.c_str(), ImVec4(0.62f, 0.92f, 0.78f, 1.f));
            //                    }
            //                    else
            //                    {
            //                        SetWaypointMessage("Teleport failed. Avatar not found.", ImVec4(0.97f, 0.48f, 0.48f, 1.f));
            //                    }
            //                }
            //                if (warpDisabled) ImGui::EndDisabled();

            //                ImGui::SameLine(0.f, 12.f);
            //                std::string copyBtnId = "COPY##" + std::to_string(i);
            //                if (ImGui::Button(copyBtnId.c_str(), copySize))
            //                {
            //                    char buffer[96];
            //                    std::snprintf(buffer, sizeof(buffer), "(%.3f;%.3f;%.3f)", entry.coords.x, entry.coords.y, entry.coords.z);
            //                    ImGui::SetClipboardText(buffer);
            //                    SetWaypointMessage("Coordinates copied.", ImVec4(0.72f, 0.86f, 1.f, 1.f));
            //                }
            //                y += ImMax(warpSize.y, copySize.y) + 8.f;

            //                std::string renderId = "wp_render_" + std::to_string(i);
            //                float renderRow = ToggleTextRow(renderId.c_str(), "Marker", entry.enabled ? "Enabled" : "Disabled",
            //                    entry.enabled, ImVec2(x, y), width);
            //                y += renderRow + 6.f;

            //                std::string deleteId = "wp_delete_" + std::to_string(i);
            //                float deleteRow = ToggleTextRow(deleteId.c_str(), "Delete", entry.deleteArmed ? "Armed" : "Safe",
            //                    entry.deleteArmed, ImVec2(x, y), width);
            //                y += deleteRow + 6.f;

            //                if (entry.deleteArmed)
            //                {
            //                    ImVec2 confirmSize(120.f, 28.f);
            //                    ImGui::SetCursorScreenPos(ImVec2(x, y));
            //                    std::string confirmBtnId = "CONFIRM##" + std::to_string(i);
            //                    if (ImGui::Button(confirmBtnId.c_str(), confirmSize))
            //                    {
            //                        removeIndex = static_cast<int>(i);
            //                    }
            //                    ImGui::SameLine(0.f, 12.f);
            //                    std::string cancelBtnId = "CANCEL##" + std::to_string(i);
            //                    if (ImGui::Button(cancelBtnId.c_str(), ImVec2(90.f, 28.f)))
            //                        entry.deleteArmed = false;
            //                    y += confirmSize.y + 4.f;
            //                }

            //                return (y - detailStart.y) + 12.f;
            //            },
            //            badge, &accent);
            //    }

            //    if (removeIndex >= 0 && removeIndex < static_cast<int>(entries.size()))
            //    {
            //        entries.erase(entries.begin() + removeIndex);
            //        SetWaypointMessage("Waypoint removed.", ImVec4(0.95f, 0.74f, 0.28f, 1.f));
            //    }*/

            //    bottom = ImMax(leftY, rightY);
            //}
            //RenderWaypointDockTab();
            ImGui::SetCursorScreenPos(ImVec2(panelPos.x, bottom));
            ImGui::Dummy(ImVec2(panelWidth, 1.f));

            float contentHeight = bottom - panelY;
            float availHeight = io.DisplaySize.y - basePanelY - 12.f;
            float maxScroll = ImMax(0.f, contentHeight - availHeight);
            if (g_menuScroll > maxScroll)
                g_menuScroll = maxScroll;
        }


    
    

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

void GuiInit(ImGuiIO& io)
{
    if (g_guiStyleApplied)
        return;
    io.IniFilename = nullptr;
    ApplyGuiStyle();
    g_guiStyleApplied = true;
}
