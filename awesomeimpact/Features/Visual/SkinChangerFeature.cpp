#include "SkinChangerFeature.h"

#include "../../GUI/gui.h"
#include "../../GUI/imgui_internal.h"
#include "../../Offsets.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Logger/Logger.h"

static SkinChangerFeature* self{ nullptr };
SkinChangerFeature::SkinChangerFeature()
	: Feature({ "card_skins", XS("Skin Changer"), XS("Change costumes and wings"), "NONE", FeatureTab::Visual })
{
	self = this;
}


namespace {

	void applyCostume(uint32_t costumeId)
	{
		auto entity = OtherUtils::AvatarManager();
		if (!entity) { ; return; }

		void* vcCostume = UnityUtils::VCCostume_GetVCCostume(entity);
		if (!vcCostume) { ; return; }

		UnityUtils::VCCostume_RefreshCostumeInfo(vcCostume, costumeId);
	}

	void applyFlycloak(uint32_t flycloakId)
	{
		auto entity = OtherUtils::AvatarManager();
		if (!entity) { ; return; }

		auto flyComp = OtherUtils::FindProp(entity, UnityUtils::GetAllVisualComponents(entity), стринги_типо::VCFlyAttachment());
		if (!flyComp) { ; return; }

		UnityUtils::VCFlyAttachment_ChangeFlycloak(flyComp, flycloakId);
		return;
	}

	uint32_t parseBuf(const char* buf)
	{
		if (!buf || !buf[0]) return 0;
		return static_cast<uint32_t>(std::strtoul(buf, nullptr, 10));
	}

}



float SkinChangerFeature::OnGUI(const ImVec2& detailStart, float width)
{
	float x = detailStart.x + 20.f;
	float y = detailStart.y + 12.f;
	float innerW = width - 40.f;

	ImDrawList* dl = ImGui::GetWindowDrawList();
	ImDrawListSplitter splitter;
	splitter.Split(dl, 2);
	splitter.SetCurrentChannel(dl, 1);

	const float fontSize = ImGui::GetFontSize();
	const float fieldHeight = fontSize + 14.f;
	const float rounding = fieldHeight * 0.5f;
	const float badgeW = fontSize + 8.f;
	const float separatorX = x + innerW * 0.5f;
	const float inputYOffset = (fieldHeight - fontSize) * 0.5f;

	ImVec2 capsuleMin(x, y);
	ImVec2 capsuleMax(x + innerW, y + fieldHeight);

	ImVec2 leftBadgeMin = capsuleMin;
	ImVec2 leftBadgeMax(capsuleMin.x + badgeW, capsuleMax.y);

	ImVec2 rightBadgeMin(capsuleMax.x - badgeW, capsuleMin.y);
	ImVec2 rightBadgeMax = capsuleMax;

	float leftInputX = leftBadgeMax.x + 4.f;
	float leftInputW = separatorX - leftInputX - 4.f;

	float rightInputX = separatorX + 4.f;
	float rightInputW = rightBadgeMin.x - rightInputX - 4.f;

	ImGui::PushID("skin_capsule");

	ImGui::SetCursorScreenPos(leftBadgeMin);
	ImGui::InvisibleButton("##apply_fly", ImVec2(badgeW, fieldHeight));
	bool leftBtnHovered = ImGui::IsItemHovered();
	bool leftBtnHeld = ImGui::IsItemActive();
	if (ImGui::IsItemClicked())
	{
		uint32_t flyId = parseBuf(flycloakBuf_.data());
		if (flyId > 0)
			RunOnUnityThread([flyId]() { applyFlycloak(flyId); });
	}

	ImGui::SetCursorScreenPos(ImVec2(leftInputX, y));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.f, inputYOffset));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushItemWidth(leftInputW);
	ImGui::InputText("##fly_id", flycloakBuf_.data(), flycloakBuf_.size(),
		ImGuiInputTextFlags_CharsDecimal);
	bool leftHovered = ImGui::IsItemHovered();
	bool leftActive = ImGui::IsItemActive();
	ImGui::PopItemWidth();
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);

	ImGui::SetCursorScreenPos(ImVec2(rightInputX, y));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.f, inputYOffset));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushItemWidth(rightInputW);
	ImGui::InputText("##cos_id", costumeBuf_.data(), costumeBuf_.size(),
		ImGuiInputTextFlags_CharsDecimal);
	bool rightHovered = ImGui::IsItemHovered();
	bool rightActive = ImGui::IsItemActive();
	ImGui::PopItemWidth();
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);

	ImGui::SetCursorScreenPos(rightBadgeMin);
	ImGui::InvisibleButton("##apply_cos", ImVec2(badgeW, fieldHeight));
	bool rightBtnHovered = ImGui::IsItemHovered();
	bool rightBtnHeld = ImGui::IsItemActive();
	if (ImGui::IsItemClicked())
	{
		uint32_t cosId = parseBuf(costumeBuf_.data());
		if (cosId > 0)
			RunOnUnityThread([cosId]() { applyCostume(cosId); });
	}

	if (leftBtnHovered || rightBtnHovered)
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

	splitter.SetCurrentChannel(dl, 0);

	ImVec4 baseFill = ImLerp(GuiColorFrame(), ImVec4(0.f, 0.f, 0.f, GuiColorFrame().w), 0.2f);
	ImVec4 borderBase = ImLerp(GuiColorFrame(), GuiColorAccent(), 0.2f);

	dl->AddRectFilled(capsuleMin, capsuleMax,
		ImGui::ColorConvertFloat4ToU32(baseFill), rounding);

	if (leftHovered || leftActive)
	{
		ImVec4 hFill = ImLerp(baseFill, ImVec4(1.f, 1.f, 1.f, 0.15f), 0.15f);
		dl->AddRectFilled(leftBadgeMax, ImVec2(separatorX, capsuleMax.y),
			ImGui::ColorConvertFloat4ToU32(hFill));
	}
	if (rightHovered || rightActive)
	{
		ImVec4 hFill = ImLerp(baseFill, ImVec4(1.f, 1.f, 1.f, 0.15f), 0.15f);
		dl->AddRectFilled(ImVec2(separatorX, capsuleMin.y), rightBadgeMin,
			ImGui::ColorConvertFloat4ToU32(hFill));
	}

	if (leftBtnHeld || leftBtnHovered)
	{
		ImVec4 bf = leftBtnHeld ? ImVec4(1.f, 1.f, 1.f, 0.18f) : ImVec4(1.f, 1.f, 1.f, 0.10f);
		dl->AddRectFilled(leftBadgeMin, leftBadgeMax,
			ImGui::ColorConvertFloat4ToU32(bf), rounding,
			ImDrawFlags_RoundCornersLeft);
	}
	if (rightBtnHeld || rightBtnHovered)
	{
		ImVec4 bf = rightBtnHeld ? ImVec4(1.f, 1.f, 1.f, 0.18f) : ImVec4(1.f, 1.f, 1.f, 0.10f);
		dl->AddRectFilled(rightBadgeMin, rightBadgeMax,
			ImGui::ColorConvertFloat4ToU32(bf), rounding,
			ImDrawFlags_RoundCornersRight);
	}

	dl->AddRect(capsuleMin, capsuleMax,
		ImGui::ColorConvertFloat4ToU32(borderBase), rounding, 0, 1.1f);

	ImU32 divCol = IM_COL32(86, 86, 100, 210);
	dl->AddLine(ImVec2(leftBadgeMax.x, capsuleMin.y + 6.f),
		ImVec2(leftBadgeMax.x, capsuleMax.y - 6.f), divCol, 1.1f);
	dl->AddLine(ImVec2(separatorX, capsuleMin.y + 6.f),
		ImVec2(separatorX, capsuleMax.y - 6.f), divCol, 1.1f);
	dl->AddLine(ImVec2(rightBadgeMin.x, capsuleMin.y + 6.f),
		ImVec2(rightBadgeMin.x, capsuleMax.y - 6.f), divCol, 1.1f);

	splitter.SetCurrentChannel(dl, 1);

	ImFont* badgeFont = ImGui::GetFont();
	float badgeFontSize = ImGui::GetFontSize();

	ImU32 fCol = leftBtnHeld    ? IM_COL32(255, 255, 255, 255)
	           : leftBtnHovered ? IM_COL32(230, 230, 240, 255)
	                            : IM_COL32(180, 180, 195, 210);
	ImVec2 fSize = badgeFont->CalcTextSizeA(badgeFontSize, FLT_MAX, 0.f, "F");
	ImVec2 fPos(leftBadgeMin.x + (badgeW - fSize.x) * 0.5f,
		leftBadgeMin.y + (fieldHeight - fSize.y) * 0.5f);
	dl->AddText(badgeFont, badgeFontSize, fPos, fCol, "F");

	ImU32 sCol = rightBtnHeld    ? IM_COL32(255, 255, 255, 255)
	           : rightBtnHovered ? IM_COL32(230, 230, 240, 255)
	                             : IM_COL32(180, 180, 195, 210);
	ImVec2 sSize = badgeFont->CalcTextSizeA(badgeFontSize, FLT_MAX, 0.f, "S");
	ImVec2 sPos(rightBadgeMin.x + (badgeW - sSize.x) * 0.5f,
		rightBadgeMin.y + (fieldHeight - sSize.y) * 0.5f);
	dl->AddText(badgeFont, badgeFontSize, sPos, sCol, "S");

	splitter.Merge(dl);
	ImGui::PopID();

	y += fieldHeight + 10.f;

	if (!statusText_.empty())
	{
		ImGui::SetCursorScreenPos(ImVec2(x, y));
		bool failed = (statusText_.find(XS("not found")) != std::string::npos ||
		               statusText_ == XS("No avatar"));
		ImVec4 col = failed ? ImVec4(1.f, 0.4f, 0.4f, 0.9f)
		                    : ImVec4(0.6f, 1.f, 0.6f, 0.75f);
		ImGui::PushStyleColor(ImGuiCol_Text, col);
		ImGui::TextUnformatted(statusText_.c_str());
		ImGui::PopStyleColor();
		y += ImGui::GetFontSize() + 8.f;
	}

	return (y - detailStart.y) + 12.f;
}

void SkinChangerFeature::OnShutdown()
{
	self->SetEnabled(false);
}
