#include "OpenPose.h"
#include "../../GUI/gui.h"
#include "../../GUI/imgui_internal.h"
#include "../../globals.h"

#include <string>
#include <vector>

extern ImFont* tabFont;

OpenPoseFeature* self{ nullptr };

namespace
{
    struct BonePoseEntry
    {
        void* transform = nullptr;
        std::string name;

        Vector3 originalAngles{};
        Vector3 originalScale{};
        Vector3 originalPosition{};

        Vector3 angles{};
        Vector3 scale{};
        Vector3 position{};

        bool expanded = false;
    };

    static std::vector<BonePoseEntry> g_boneEntries;
    static float g_openPoseSlideT = 0.0f;

    static void ScanAvatarBones()
    {
        g_boneEntries.clear();

        void* root = BoneUtils::GetLocalAvatarRootTransform();
        if (!root)
            return;

        BoneUtils::ForEachBone(root, "", false, [&](const BoneUtils::BoneData& b)
            {
                if (!b.transform)
                    return;

                BonePoseEntry entry;
                entry.transform = b.transform;
                entry.name = b.name.empty() ? XS("UnnamedBone") : b.name;

                entry.originalAngles = UnityUtils::Transform_get_localEulerAngles(b.transform);
                entry.originalScale = UnityUtils::Transform_get_localScale(b.transform);
                entry.originalPosition = UnityUtils::Transform_get_localPosition(b.transform);

                entry.angles = entry.originalAngles;
                entry.scale = entry.originalScale;
                entry.position = entry.originalPosition;

                g_boneEntries.push_back(std::move(entry));
            });
    }

    static void ResetAvatarBonesToScanned()
    {
        for (auto& entry : g_boneEntries)
        {
            if (!entry.transform)
                continue;

            entry.angles = entry.originalAngles;
            entry.scale = entry.originalScale;
            entry.position = entry.originalPosition;

            UnityUtils::Transform_set_localEulerAngles(entry.transform, entry.originalAngles);
            UnityUtils::Transform_set_localScale(entry.transform, entry.originalScale);
            UnityUtils::Transform_set_localPosition(entry.transform, entry.originalPosition);
        }
    }

    static bool DrawVectorEditor(const char* id, const char* sectionTitle, Vector3& value, float speed, float rowWidth)
    {
        ImVec2 rowPos = ImGui::GetCursorScreenPos();
        const float textH = ImGui::GetTextLineHeight();
        const float frameH = ImGui::GetFrameHeight();
        const float rowH = ImMax(textH, frameH) + 2.0f;

        float inputWidth = ImClamp(rowWidth * 0.68f, 150.0f, rowWidth - 84.0f);
        float inputX = rowPos.x + rowWidth - inputWidth;

        ImGui::SetCursorScreenPos(ImVec2(rowPos.x, rowPos.y + (rowH - textH) * 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.72f));
        ImGui::TextUnformatted(sectionTitle);
        ImGui::PopStyleColor();

        ImGui::SetCursorScreenPos(ImVec2(inputX, rowPos.y));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 0.0f);
        ImGui::PushItemWidth(inputWidth);
        bool changed = ImGui::DragFloat3(id, &value.x, speed, 0.0f, 0.0f, "%.3f");
        ImGui::PopItemWidth();
        ImGui::PopStyleVar(2);

        ImGui::SetCursorScreenPos(ImVec2(rowPos.x, rowPos.y + rowH));
        return changed;
    }
}

OpenPoseFeature::OpenPoseFeature()
    : Feature({ "card_open_pose", XS("Open Pose"), XS("Scan bones for editing"), "NONE", FeatureTab::Misc })
{
    self = this;
}

float OpenPoseFeature::OnGUI(const ImVec2& detailStart, float width)
{
    float x = detailStart.x + 20.f;
    float y = detailStart.y + 16.f;
    ImGui::SetCursorScreenPos(ImVec2(x, y));
    ImGui::TextUnformatted(XS("Opens side panel with bone scanner"));
    y += ImGui::GetFontSize() + 10.f;
    float holdRow = GuiToggleTextRow("open_pose_hold", XS("Key"), XS("Hold"), hold_, ImVec2(x, y), width);
    y += holdRow + 6.f;
    return (y - detailStart.y) + 12.f;
}

void OpenPoseFeature::OnDraw()
{
    if (!self || !self->Active())
    {
        menuVisible_ = false;
        overlayExpanded_ = false;
        g_openPoseSlideT = 0.0f;
        return;
    }

    menuVisible_ = true;

    ImGuiIO& io = ImGui::GetIO();
    const float panelWidth = 360.f;
    const float collapsedWidth = 48.f;
    const float panelHeight = 520.f;

    float targetX = io.DisplaySize.x - panelWidth;
    float y = io.DisplaySize.y * 0.25f;

    ImVec2 size(panelWidth, panelHeight);
    float offsetX = (1.0f - g_openPoseSlideT) * (panelWidth - collapsedWidth);
    ImVec2 pos(targetX + offsetX, y);

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    if (ImGui::Begin(XS("OpenPoseOverlay"), nullptr, flags))
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec2 winPos = ImGui::GetWindowPos();
        ImVec2 winSize = ImGui::GetWindowSize();
        ImVec2 bgMin = winPos;
        ImVec2 bgMax = ImVec2(winPos.x + winSize.x, winPos.y + winSize.y);
        float cornerRadius = 10.f;
#ifdef ImDrawFlags_RoundCornersLeft
        ImDrawFlags cornerFlags = ImDrawFlags_RoundCornersLeft;
#else
        ImDrawFlags cornerFlags = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBottomLeft;
#endif
        ImU32 fillCol = ImGui::GetColorU32(style.Colors[ImGuiCol_WindowBg]);
        ImU32 borderCol = ImGui::GetColorU32(style.Colors[ImGuiCol_Border]);
        dl->AddRectFilled(bgMin, bgMax, fillCol, cornerRadius, cornerFlags);
        dl->AddRect(bgMin, bgMax, borderCol, cornerRadius, cornerFlags, 1.0f);
        ImGui::PushClipRect(bgMin, bgMax, true);

        ImVec2 mouse = io.MousePos;
        float hoverWidth = ImLerp(collapsedWidth, panelWidth, g_openPoseSlideT);
        bool hovered = (mouse.x >= pos.x && mouse.x <= pos.x + hoverWidth &&
            mouse.y >= pos.y && mouse.y <= pos.y + panelHeight);
        float targetSlide = hovered ? 1.0f : 0.0f;
        float speed = ImClamp(io.DeltaTime * 12.0f, 0.0f, 1.0f);
        g_openPoseSlideT = ImLerp(g_openPoseSlideT, targetSlide, speed);
        overlayExpanded_ = g_openPoseSlideT > 0.05f;

        bool open = g_openPoseSlideT > 0.5f;
        ImVec2 barMin(winPos.x + 8.f, winPos.y + winSize.y * 0.5f - 14.f);
        ImVec2 barMax(barMin.x + 4.f, barMin.y + 28.f);
        float barAlpha = open ? 0.78f : 1.0f;
        dl->AddRectFilled(barMin, barMax, IM_COL32(255, 255, 255, (int)(255 * barAlpha)), 2.f);

        ImVec2 cur = ImGui::GetCursorScreenPos();
        float barAlignedX = barMin.x + 10.f;
        float contentX = winPos.x + 38.f;
        float contentWidth = size.x - (contentX - winPos.x) - 24.f;
        float yy = cur.y + 12.f;

        ImGui::SetCursorScreenPos(ImVec2(barAlignedX, yy));
        if (tabFont) ImGui::PushFont(tabFont);
        ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 0.90f), XS("Open Pose"));
        if (tabFont) ImGui::PopFont();
        yy += ImGui::GetFontSize() + 10.f;

        const float actionHeight = ImGui::GetFontSize() + 12.f;
        const float blockPad = 8.f;
        float blockHeight = (winPos.y + panelHeight) - yy - 12.f;
        if (blockHeight < actionHeight * 2.f + 28.f)
            blockHeight = actionHeight * 2.f + 28.f;

        ImVec2 blockMin(contentX, yy);
        ImVec2 blockMax(contentX + contentWidth, yy + blockHeight);
        float blockRounding = 10.f;

        ImVec4 blockFill = ImLerp(GuiColorFrame(), ImVec4(0.f, 0.f, 0.f, GuiColorFrame().w), 0.28f);
        ImVec4 blockBorder = ImLerp(GuiColorFrame(), GuiColorAccent(), 0.22f);
        dl->AddRectFilled(blockMin, blockMax, ImGui::ColorConvertFloat4ToU32(blockFill), blockRounding);
        dl->AddRect(blockMin, blockMax, ImGui::ColorConvertFloat4ToU32(blockBorder), blockRounding, 0, 1.1f);

        ImVec2 scanPos = blockMin;
        ImVec2 scanMax(blockMax.x, blockMin.y + actionHeight);
        ImGui::SetCursorScreenPos(scanPos);
        bool scanPressed = ImGui::InvisibleButton("##open_pose_scan", ImVec2(contentWidth, actionHeight));
        bool scanHovered = ImGui::IsItemHovered();
        if (scanPressed)
            ScanAvatarBones();

        if (scanHovered)
        {
            ImU32 scanHoverCol = ImGui::ColorConvertFloat4ToU32(
                ImVec4(GuiColorAccent().x, GuiColorAccent().y, GuiColorAccent().z, 0.14f));
#ifdef ImDrawFlags_RoundCornersTop
            dl->AddRectFilled(scanPos, scanMax, scanHoverCol, blockRounding, ImDrawFlags_RoundCornersTop);
#else
            dl->AddRectFilled(scanPos, scanMax, scanHoverCol, blockRounding);
#endif
        }
        dl->AddLine(ImVec2(scanPos.x + 8.f, scanMax.y), ImVec2(scanMax.x - 8.f, scanMax.y), IM_COL32(86, 86, 100, 170), 1.0f);

        ImVec2 scanTextSize = ImGui::CalcTextSize(XS("Scan"));
        float scanTextX = scanPos.x + (contentWidth - scanTextSize.x) * 0.5f;
        ImVec2 scanTextPos(scanTextX, scanPos.y + (actionHeight - scanTextSize.y) * 0.5f - 0.5f);
        dl->AddText(scanTextPos, IM_COL32(240, 240, 248, 255), XS("Scan"));

        ImVec2 resetPos(blockMin.x, blockMax.y - actionHeight);
        ImVec2 resetMax(blockMax.x, blockMax.y);
        ImGui::SetCursorScreenPos(resetPos);
        bool resetPressed = ImGui::InvisibleButton("##open_pose_reset", ImVec2(contentWidth, actionHeight));
        bool resetHovered = ImGui::IsItemHovered();
        if (resetPressed)
            ResetAvatarBonesToScanned();

        if (resetHovered)
        {
            ImU32 resetHoverCol = ImGui::ColorConvertFloat4ToU32(
                ImVec4(GuiColorAccent().x, GuiColorAccent().y, GuiColorAccent().z, 0.14f));
#ifdef ImDrawFlags_RoundCornersBottom
            dl->AddRectFilled(resetPos, resetMax, resetHoverCol, blockRounding, ImDrawFlags_RoundCornersBottom);
#else
            dl->AddRectFilled(resetPos, resetMax, resetHoverCol, blockRounding);
#endif
        }
        dl->AddLine(ImVec2(resetPos.x + 8.f, resetPos.y), ImVec2(resetMax.x - 8.f, resetPos.y), IM_COL32(86, 86, 100, 170), 1.0f);

        ImVec2 resetTextSize = ImGui::CalcTextSize(XS("Reset"));
        float resetTextX = resetPos.x + (contentWidth - resetTextSize.x) * 0.5f;
        ImVec2 resetTextPos(resetTextX, resetPos.y + (actionHeight - resetTextSize.y) * 0.5f - 0.5f);
        dl->AddText(resetTextPos, IM_COL32(240, 240, 248, 255), XS("Reset"));

        float listTop = scanMax.y + 6.f;
        float listBottom = resetPos.y - 6.f;
        ImVec2 listPos(blockMin.x + blockPad, listTop);
        ImVec2 listSize(contentWidth - blockPad * 2.f, listBottom - listTop);
        if (listSize.y < 8.f)
            listSize.y = 8.f;

        ImGui::SetCursorScreenPos(listPos);
        ImGuiWindowFlags listFlags = ImGuiWindowFlags_NoScrollbar;
        if (ImGui::BeginChild("##open_pose_bone_list", listSize, false, listFlags))
        {
            for (size_t i = 0; i < g_boneEntries.size(); ++i)
            {
                BonePoseEntry& entry = g_boneEntries[i];
                if (!entry.transform)
                    continue;

                ImGui::PushID((int)i);
                const float fontSize = ImGui::GetFontSize();
                const float rowHeight = fontSize + 10.f;
                const float rowGap = 5.f;
                const float bodyPadX = 8.f;
                const float bodyPadY = 6.f;
                const float sectionGap = 6.f;
                const float sectionHeight = ImMax(ImGui::GetTextLineHeight(), ImGui::GetFrameHeight()) + 2.f;
                const float bodyHeight = bodyPadY * 2.f + sectionHeight * 3.f + sectionGap * 2.f;
                const float expandedHeight = entry.expanded ? bodyHeight : 0.f;
                const float frameHeight = rowHeight + expandedHeight;
                const float totalHeight = frameHeight + rowGap;

                ImVec2 itemPos = ImGui::GetCursorScreenPos();
                ImGui::Dummy(ImVec2(listSize.x, totalHeight));

                ImGui::SetCursorScreenPos(itemPos);
                bool pressed = ImGui::InvisibleButton("##bone_row", ImVec2(listSize.x, rowHeight));
                bool hoveredRow = ImGui::IsItemHovered();
                bool heldRow = ImGui::IsItemActive();
                if (pressed)
                    entry.expanded = !entry.expanded;

                ImDrawList* listDl = ImGui::GetWindowDrawList();
                ImVec2 frameMin = itemPos;
                ImVec2 frameMax(itemPos.x + listSize.x, itemPos.y + frameHeight);
                ImVec2 rowMin = frameMin;
                ImVec2 rowMax(frameMax.x, frameMin.y + rowHeight);

                ImVec4 rowBase = ImLerp(GuiColorFrame(), ImVec4(0.f, 0.f, 0.f, GuiColorFrame().w), 0.15f);
                ImVec4 rowHover = ImLerp(rowBase, GuiColorAccent(), 0.16f);
                ImVec4 rowFill = rowBase;
                if (entry.expanded)
                    rowFill = ImLerp(rowFill, rowHover, 0.22f);
                if (hoveredRow)
                    rowFill = ImLerp(rowFill, rowHover, 0.75f);
                if (heldRow)
                    rowFill = ImLerp(rowFill, ImVec4(GuiColorAccent().x, GuiColorAccent().y, GuiColorAccent().z, 0.85f), 0.3f);
                ImVec4 rowBorder = ImLerp(GuiColorFrame(), GuiColorAccent(), (entry.expanded || hoveredRow) ? 0.45f : 0.18f);

                listDl->AddRectFilled(frameMin, frameMax, ImGui::ColorConvertFloat4ToU32(rowFill), 2.f);
                listDl->AddRect(frameMin, frameMax, ImGui::ColorConvertFloat4ToU32(rowBorder), 2.f, 0, 1.0f);
                if (entry.expanded)
                {
                    listDl->AddLine(
                        ImVec2(frameMin.x + 6.f, rowMax.y),
                        ImVec2(frameMax.x - 6.f, rowMax.y),
                        IM_COL32(86, 86, 100, 170), 1.0f);
                }

                ImVec2 arrowCenter(rowMin.x + 12.f, rowMin.y + rowHeight * 0.5f);
                if (entry.expanded)
                {
                    listDl->AddTriangleFilled(
                        ImVec2(arrowCenter.x - 4.f, arrowCenter.y + 2.8f),
                        ImVec2(arrowCenter.x + 4.f, arrowCenter.y + 2.8f),
                        ImVec2(arrowCenter.x, arrowCenter.y - 2.8f),
                        IM_COL32(235, 235, 245, 255));
                }
                else
                {
                    listDl->AddTriangleFilled(
                        ImVec2(arrowCenter.x - 4.f, arrowCenter.y - 2.8f),
                        ImVec2(arrowCenter.x + 4.f, arrowCenter.y - 2.8f),
                        ImVec2(arrowCenter.x, arrowCenter.y + 2.8f),
                        IM_COL32(225, 225, 235, 255));
                }

                ImVec2 textPos(rowMin.x + 24.f, rowMin.y + (rowHeight - fontSize) * 0.5f - 1.f);
                listDl->AddText(textPos, IM_COL32(238, 238, 246, 255), entry.name.c_str());

                if (entry.expanded)
                {
                    const float sectionX = frameMin.x + bodyPadX;
                    const float sectionWidth = listSize.x - bodyPadX * 2.f;
                    float sectionY = rowMax.y + bodyPadY;

                    ImGui::SetCursorScreenPos(ImVec2(sectionX, sectionY));
                    if (DrawVectorEditor("##angles", XS("Angles"), entry.angles, 0.25f, sectionWidth))
                        UnityUtils::Transform_set_localEulerAngles(entry.transform, entry.angles);
                    sectionY = ImGui::GetCursorScreenPos().y + sectionGap;

                    ImGui::SetCursorScreenPos(ImVec2(sectionX, sectionY));
                    if (DrawVectorEditor("##scale", XS("Scale"), entry.scale, 0.01f, sectionWidth))
                        UnityUtils::Transform_set_localScale(entry.transform, entry.scale);
                    sectionY = ImGui::GetCursorScreenPos().y + sectionGap;

                    ImGui::SetCursorScreenPos(ImVec2(sectionX, sectionY));
                    if (DrawVectorEditor("##position", XS("Position"), entry.position, 0.01f, sectionWidth))
                        UnityUtils::Transform_set_localPosition(entry.transform, entry.position);
                }

                ImGui::SetCursorScreenPos(ImVec2(itemPos.x, itemPos.y + totalHeight));
                ImGui::PopID();
            }
        }
        ImGui::EndChild();

        ImGui::SetCursorScreenPos(ImVec2(winPos.x, winPos.y));
        ImGui::Dummy(ImVec2(winSize.x, winSize.y));

        ImGui::PopClipRect();
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
}

void OpenPoseFeature::OnShutdown()
{
    menuVisible_ = false;
    overlayExpanded_ = false;
    g_openPoseSlideT = 0.0f;
    g_boneEntries.clear();
    self->SetEnabled(false);
}
