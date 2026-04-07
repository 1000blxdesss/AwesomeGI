#include "ArrayList.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../GUI/imgui_internal.h"
#include "../../Utils/test02.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

static ArrayListFeature* self{ nullptr };
ArrayListFeature::ArrayListFeature()
    : Feature({ "card_array_list", XS("Array List"), XS("Show active features list"), "NONE", FeatureTab::Visual })
{
    self = this;
}

namespace {

    struct ArrayItem {
        std::string name;
        std::string mode;
        float       width;
    };

    inline std::vector<ArrayItem> CollectItems()
    {
        std::vector<ArrayItem> items;

        for (const auto& entry : GetFeatureManager().All())
        {
            auto* feature = entry.get();
            if (!feature || !feature->Active())
                continue;

            const auto& title = feature->Info().title;
            const char* bracket = std::strchr(title, '[');

            std::string base = bracket ? std::string(title, bracket) : std::string(title);
            std::string mode = bracket ? std::string(bracket) : std::string();

            const float w = ImGui::CalcTextSize(base.c_str()).x + ImGui::CalcTextSize(mode.c_str()).x;
            items.push_back({ std::move(base), std::move(mode), w });
        }

        std::stable_sort(items.begin(), items.end(), [](const ArrayItem& a, const ArrayItem& b) {
            return a.width != b.width ? a.width > b.width : a.name < b.name;
            });

        return items;
    }

    inline void RenderOverlay()
    {
        auto* dl = ImGui::GetForegroundDrawList();
        if (!dl) return;

        const auto items = CollectItems();
        if (items.empty()) return;

        constexpr float kPadX = 8.f;
        constexpr float kPadY = 4.f;
        constexpr float kOverlap = 5.f;
        constexpr float kRound = 6.f;

        const float fontSize = ImGui::GetFontSize();
        const float boxHeight = kPadY * 2.f + fontSize;
        const float anchorX = ImGui::GetIO().DisplaySize.x;

        const ImU32 bg = IM_COL32(0, 0, 0, 255);
        const ImU32 text = IM_COL32(230, 230, 238, 240);
        const ImU32 shadow = IM_COL32(0, 0, 0, 200);

        const auto accentF = GuiActiveAccent();
        const ImU32 accent = IM_COL32(
            static_cast<int>(accentF.x * 255.f),
            static_cast<int>(accentF.y * 255.f),
            static_cast<int>(accentF.z * 255.f), 240);

        float y = 0.f;

        for (const auto& item : items)
        {
            const float boxWidth = item.width + kPadX * 2.f;
            const ImVec2 rectMin = { anchorX - boxWidth, y };
            const ImVec2 rectMax = { anchorX, y + boxHeight + kOverlap };

            dl->AddRectFilled(rectMin, rectMax, bg, kRound, ImDrawFlags_RoundCornersLeft);

            const float textX = rectMax.x - kPadX - item.width;
            const ImVec2 basePos = { textX, rectMin.y + kPadY };
            const ImVec2 shadowOff = { 1.f, 1.f };

            dl->AddText(add(basePos, shadowOff), shadow, item.name.c_str());
            dl->AddText(basePos, text, item.name.c_str());

            if (!item.mode.empty())
            {
                const ImVec2 modePos = { basePos.x + ImGui::CalcTextSize(item.name.c_str()).x, basePos.y };
                dl->AddText(add(modePos, shadowOff), shadow, item.mode.c_str());
                dl->AddText(modePos, accent, item.mode.c_str());
            }

            y += boxHeight - kOverlap;
        }
    }
}

float ArrayListFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiToggleTextRow("array_list_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void ArrayListFeature::OnDraw()
{
    if (!self->Active()) return;
    RenderOverlay();
}

void ArrayListFeature::OnShutdown()
{
    SetEnabled(false);
}