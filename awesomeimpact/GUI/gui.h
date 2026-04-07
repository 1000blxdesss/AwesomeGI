#include "imgui.h"
#pragma once

#include <array>
#include <string>
#include <vector>

class Feature;

struct CapsuleEntry
{
    std::string label;
    ImVec4 color = ImVec4(0.48f, 0.48f, 0.86f, 1.f);
};

struct TextureTargetEntry
{
    std::string rendererName;
    std::string displayName;
    std::array<char, 260> path{};
    void* material{};
    void* renderer{};
    int matIndex{};
};

void GuiInit(ImGuiIO& io);
void GuiRender();
float GuiToggleTextRow(const char* id, const char* label, const char* valueText, bool& state,
    const ImVec2& origin, float innerWidth);
float GuiEnabledDisabledRow(const char* id, const char* label, bool& state,
    const ImVec2& origin, float innerWidth);
float GuiFeatureEnabledRow(const char* id, const char* label, Feature& feature,
    const ImVec2& origin, float innerWidth);
float GuiRangeSliderRow(const char* id, const char* label, float* v_min, float* v_max, float v_lower, float v_upper,
    const char* display_fmt, const ImVec2& origin, float innerWidth, bool brightLabel);
float GuiDualListInputRow(const char* id,
    const char* leftLabel, const char* rightLabel,
    char* leftBuffer, size_t leftBufferSize,
    char* rightBuffer, size_t rightBufferSize,
    bool& leftAddTriggered, bool& rightAddTriggered,
    const ImVec2& origin, float innerWidth);
float GuiInputAddRow(const char* id, const char* label, char* buffer, size_t bufferSize,
    bool& addTriggered, const ImVec2& origin, float innerWidth);
float GuiInputConfirmRow(const char* id, const char* label, char* buffer, size_t bufferSize,
    bool& confirmed, const ImVec2& origin, float innerWidth);
float GuiInputActionConfirmRow(const char* id, const char* label, char* buffer, size_t bufferSize,
    char leftGlyph, bool& leftTriggered, bool& confirmed, const ImVec2& origin, float innerWidth);
float GuiDropdownRow(const char* id, const char* label, const char* const* options, int optionCount,
    int& currentIndex, const ImVec2& origin, float innerWidth);
float GuiSliderRow(const char* id, const char* label, float* value, float v_min, float v_max, const char* display_fmt,
    const ImVec2& origin, float innerWidth, bool brightLabel = false);
float GuiCapsuleListRow(const char* id, std::vector<CapsuleEntry>& entries,
    const ImVec2& origin, float innerWidth);
float GuiCapsuleListRowEx(const char* id, std::vector<CapsuleEntry>& entries,
    const ImVec2& origin, float innerWidth, int* clickedIndex,
    bool allowContextEdit = true, bool edgeCloseStyle = false);
float GuiColorButtonRow(const char* id, const char* label, ImVec4& color,
    const ImVec2& origin, float innerWidth, bool enablePicker = true);
float GuiTextureInputRow(const char* id, const char* label, TextureTargetEntry& entry,
    bool& applyTriggered, bool& resetTriggered, const ImVec2& origin, float innerWidth);
float GuiDetailRowText(const char* label, const char* value, const ImVec2& origin, float innerWidth, bool drawSeparator);
void GuiCustomCheckbox(const char* id, const char* label, bool& value, const ImVec2& pos);
void GuiAddCapsuleTokens(const std::string& raw, std::vector<CapsuleEntry>& target, const ImVec4& defaultColor);
void GuiSetNextHeaderSplitBadge(const char* cardId, int* value, const char* leftLabel, const char* rightLabel, bool* linkedBool = nullptr);
void GuiBindConfigBool(const char* id, bool* value);
void GuiBindConfigInt(const char* id, int* value);
void GuiBindConfigFloat(const char* id, float* value);
void GuiBindConfigString(const char* id, char* value, size_t size);
void GuiBindConfigColor(const char* id, ImVec4* value);

ImVec4& GuiColorWindow();
ImVec4& GuiColorChild();
ImVec4& GuiColorFrame();
ImVec4& GuiColorButton();
ImVec4& GuiColorText();
ImVec4& GuiColorTextEnabled();
ImVec4& GuiColorTextDisabled();
ImVec4& GuiColorAccent();
ImVec4& GuiColorOutline();
ImVec4& GuiColorHighlight();
ImVec4& GuiColorBadge();
ImVec4& GuiOverlayColor();
const ImVec4& GuiActiveAccent();
void GuiApplyStyle();
void GuiResetColors();

extern ImFont* g_fontNameTags;
extern bool g_showSettings;
