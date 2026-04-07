#include "ProfileChanger.h"
#include "../Misc/KeepCamera.h"
#include "../../GUI/gui.h"
#include "../../GUI/imgui_internal.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/stb_image_write.h"
#include "../../Utils/test02.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>

extern ImFont* tabFont;

static ProfileChangerFeature* self{ nullptr };
static bool profileMenuVisible = false;
ProfileChangerFeature::ProfileChangerFeature()
    : Feature({ "card_profile_changer", XS("Profile Changer"), XS("Customize player details"), "NONE", FeatureTab::Visual })
{
    self = this;
}

namespace {

    struct ProfileJob {
        enum Type { GetText, ApplyText, ApplyTexture, DumpTexture, ApplyColor } type;
        std::string field;
        std::string payload;
        char* outBuffer = nullptr;
        size_t outSize = 0;
        ImVec4 color{};
    };

    void* g_profilePage = nullptr;
    std::mutex               g_profileMutex;
    std::vector<ProfileJob>  g_profileJobs;


    struct TextField {
        const char* id;
        const char* label;
        const char* field;
        char* buffer;
        size_t      bufSize;
    };

    char profileUID[32]{};
    char profileLevel[16]{};
    char profileWorldLevel[16]{};
    char profileCurExp[32]{};
    char profileMaxExp[32]{};
    char profileInfo[32]{};
    char profileExpBar[32]{};
    char profileSign[32]{};
    char profileNoSign[32]{};
    char profileUIDd[32]{};
    char profileBirthday[32]{};
    char profileIconPath[260]{};
    char profileBgPath[260]{};

    int  profileModeIndex = 0;
    bool profileTextRestoreQueued = false;
    bool profileOverlayExpanded = false;
    ImVec4 profileColorName = { 1, 1, 1, 1 };

    const TextField kTextFields[] = {
        { "profile_uid_overlay",         XS("Name"),      "name",        profileUID,        sizeof(profileUID)        },
        { "profile_level_overlay",       XS("LVL"),       "level",       profileLevel,      sizeof(profileLevel)      },
        { "profile_world_level_overlay", XS("World LVL"), "world_level", profileWorldLevel, sizeof(profileWorldLevel) },
        { "profile_cur_exp_overlay",     XS("Cur EXP"),   "cur_exp",     profileCurExp,     sizeof(profileCurExp)     },
        { "profile_cost_overlay",        XS("Cost"),       "cost",       profileMaxExp,     sizeof(profileMaxExp)     },
        { "profile_info_overlay",        XS("Info"),       "info",       profileInfo,       sizeof(profileInfo)       },
        { "profile_num_overlay",         XS("Num"),        "num",        profileExpBar,     sizeof(profileExpBar)     },
        { "profile_sign_overlay",        XS("Sign"),       "sign",       profileSign,       sizeof(profileSign)       },
        { "profile_nosign_overlay",      XS("No Sign"),    "nosign",     profileNoSign,     sizeof(profileNoSign)     },
        { "profile_uid2_overlay",        XS("UID"),        "uid",        profileUIDd,       sizeof(profileUIDd)       },
        { "profile_bday_overlay",        XS("Birthday"),   "birthday",   profileBirthday,   sizeof(profileBirthday)   },
    };

    inline void PushJob(ProfileJob job)
    {
        std::lock_guard lock(g_profileMutex);
        g_profileJobs.push_back(std::move(job));
    }

    inline void TrimInPlace(std::string& s)
    {
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))  s.pop_back();
    }

    inline bool ParseExpPair(const std::string& input, float& cur, float& max)
    {
        const auto sep = input.find('/');
        if (sep == std::string::npos) return false;
        auto left = input.substr(0, sep);
        auto right = input.substr(sep + 1);
        TrimInPlace(left); TrimInPlace(right);
        if (left.empty() || right.empty()) return false;
        cur = std::strtof(left.c_str(), nullptr);
        max = std::strtof(right.c_str(), nullptr);
        return true;
    }

    void QueueProfileTextReapplyIfFilled()
    {
        std::lock_guard lock(g_profileMutex);
        if (profileTextRestoreQueued) return;

        for (const auto& tf : kTextFields)
            if (tf.buffer[0] != '\0')
                g_profileJobs.push_back({ ProfileJob::ApplyText, tf.field, std::string(tf.buffer) });

        profileTextRestoreQueued = true;
    }

    inline std::string GetText(void* text)
    {
        if (!text) return {};
        auto* str = UnityUtils::Text_get_text(text);
        return (str && str->chars) ? OtherUtils::Utf16ToUtf8(str->chars, str->length) : std::string();
    }

    inline void SetText(void* text, const std::string& value)
    {
        if (!text) return;
        auto* str = UnityUtils::PtrToStringAnsi((void*)value.c_str());
        if (str) UnityUtils::Text_set_text(text, str);
    }

    inline void* ResolveTextField(MonoInLevelPlayerProfilePageV3* p, const std::string& field)
    {
        if (field == "name")        return p->_playerName;
        if (field == "level")       return p->_playerLv;
        if (field == "world_level") return p->_playerWorldLv;
        if (field == "cur_exp")     return p->_playerExp;
        if (field == "cost")        return p->_playerCost;
        if (field == "info")        return p->_playerInfo;
        if (field == "num")         return p->_playerNum;
        if (field == "birthday")    return p->_playerBirthday;
        if (field == "sign")        return p->_playerSignature;
        if (field == "nosign")      return p->_playerNoSignature;
        if (field == "uid")         return p->_playerID;
        return nullptr;
    }

    bool ProfileGetTextUnity(const std::string& field, char* out, size_t outSize)
    {
        auto* p = reinterpret_cast<MonoInLevelPlayerProfilePageV3*>(g_profilePage);
        if (!p || !out || !outSize) return false;
        auto* widget = ResolveTextField(p, field);
        if (!widget) return false;
        const auto value = GetText(widget);
        if (value.empty()) return false;
        strncpy_s(out, outSize, value.c_str(), _TRUNCATE);
        return true;
    }

    bool ProfileApplyTextUnity(const std::string& field, const std::string& value)
    {
        auto* p = reinterpret_cast<MonoInLevelPlayerProfilePageV3*>(g_profilePage);
        if (!p || value.empty()) return false;

        auto* widget = ResolveTextField(p, field);
        if (!widget) return false;

        SetText(widget, value);

        if (field == "cur_exp")
        {
            auto* slider = p->_playerExpSlider;
            if (slider)
            {
                float cur = 0, mx = 0;
                if (ParseExpPair(value, cur, mx) && mx > 0.f)
                {
                    const float minV = UnityUtils::Slider_get_minValue(slider);
                    const float maxV = UnityUtils::Slider_get_maxValue(slider);
                    UnityUtils::Slider_set_value(slider, minV + (maxV - minV) * (cur / mx));
                    UnityUtils::Slider_UpdateVisuals(slider);
                }
            }
        }

        return true;
    }

    bool DumpTextureToPng(void* tex, const std::string& filename)
    {
        if (!tex) return false;
        const int w = UnityUtils::Texture_get_width(tex);
        const int h = UnityUtils::Texture_get_height(tex);
        if (w <= 0 || h <= 0) return false;

        auto* tempRT = UnityUtils::RenderTexture_GetTemporary(w, h, 0);
        if (!tempRT) return false;

        auto* prevRT = UnityUtils::RenderTexture_get_active();
        UnityUtils::Graphics_Blit(tex, tempRT);
        UnityUtils::RenderTexture_set_active(tempRT);

        auto* newTex = malloc(1024);
        if (!newTex) {
            UnityUtils::RenderTexture_set_active(prevRT);
            UnityUtils::RenderTexture_ReleaseTemporary(tempRT);
            return false;
        }
        std::memset(newTex, 0, 1024);
        *reinterpret_cast<void**>(newTex) = *reinterpret_cast<void**>(tex);
        UnityUtils::Texture2D_ctor(newTex, w, h, 4, false);

        Rect rect = { 0, 0, static_cast<float>(w), static_cast<float>(h) };
        UnityUtils::Texture2D_ReadPixels(newTex, &rect, 0, 0, false);
        UnityUtils::Texture2D_Apply(newTex);

        auto* pixels = UnityUtils::Texture2D_GetPixels32(newTex, 0);
        UnityUtils::RenderTexture_set_active(prevRT);
        UnityUtils::RenderTexture_ReleaseTemporary(tempRT);

        if (!pixels || pixels->max_length != static_cast<uint32_t>(w * h)) {
            UnityUtils::Object_Destroy(newTex);
            free(newTex);
            return false;
        }

        auto* src = reinterpret_cast<Color32*>(pixels->vector);
        std::vector<unsigned char> img(w * h * 4);
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x) {
                const int si = y * w + x;
                const int di = (h - 1 - y) * w + x;
                img[di * 4] = src[si].r;
                img[di * 4 + 1] = src[si].g;
                img[di * 4 + 2] = src[si].b;
                img[di * 4 + 3] = src[si].a;
            }

        UnityUtils::Object_Destroy(newTex);
        free(newTex);
        return stbi_write_png(filename.c_str(), w, h, 4, img.data(), w * 4) != 0;
    }

    inline void* ResolveImageWidget(MonoInLevelPlayerProfilePageV3* p, const std::string& field)
    {
        if (field == "icon")     return p->playerIconImage;
        if (field == "namecard") return p->_nameCardPic;
        return nullptr;
    }

    bool ProfileApplyTextureUnity(const std::string& field, const std::string& path)
    {
        auto* p = reinterpret_cast<MonoInLevelPlayerProfilePageV3*>(g_profilePage);
        if (!p || path.empty()) return false;

        auto* image = ResolveImageWidget(p, field);
        if (!image) return false;

        auto* sprite = UnityUtils::Image_get_activeSprite(image);
        if (!sprite) return false;
        auto* tex = UnityUtils::Sprite_get_texture(sprite);
        if (!tex) return false;

        auto* bytes = UnityUtils::File_ReadAllBytes(UnityUtils::PtrToStringAnsi((void*)path.c_str()));
        if (!bytes || bytes->max_length == 0) return false;

        if (!UnityUtils::ImageConversion_LoadImageData(tex, bytes)) return false;
        *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(image) + offsets::UnityEngine::UI_Image::m_PreserveAspect) = true;
        return true;
    }

    bool ProfileDumpTextureUnity(const std::string& field, const std::string& path)
    {
        auto* p = reinterpret_cast<MonoInLevelPlayerProfilePageV3*>(g_profilePage);
        if (!p) return false;

        auto* image = ResolveImageWidget(p, field);
        if (!image) return false;

        auto* sprite = UnityUtils::Image_get_activeSprite(image);
        if (!sprite) return false;
        auto* tex = UnityUtils::Sprite_get_texture(sprite);
        if (!tex) return false;

        const auto& outPath = path.empty()
            ? std::string(field == "icon" ? XS("icon.png") : XS("namecard.png"))
            : path;
        return DumpTextureToPng(tex, outPath);
    }

    bool ProfileApplyColorUnity(const std::string& field, const ImVec4& color)
    {
        auto* p = reinterpret_cast<MonoInLevelPlayerProfilePageV3*>(g_profilePage);
        if (!p) return false;
        if (field != "Slider") return false;

        auto* slider = p->_playerExpSlider;
        if (!slider) return false;

        auto* fillImage = *reinterpret_cast<void**>(
            reinterpret_cast<uintptr_t>(slider) + offsets::UnityEngine::UI_Slider::m_FillImage);
        if (!fillImage) return false;

        UnityUtils::Graphic_set_color(fillImage, { color.x, color.y, color.z, color.w });
        return true;
    }


    float ProfilePathRow(const char* id, const char* label, char* buffer, size_t bufferSize,
        char leftGlyph, bool& leftTriggered, bool& applyTriggered,
        const ImVec2& origin, float innerWidth)
    {
        auto* dl = ImGui::GetWindowDrawList();
        ImDrawListSplitter splitter;
        splitter.Split(dl, 2);
        splitter.SetCurrentChannel(dl, 1);

        leftTriggered = applyTriggered = false;

        const float fontSize = ImGui::GetFontSize();
        const float fieldHeight = fontSize + 10.f;
        const float rowHeight = fieldHeight + 8.f;
        constexpr float kBtnW = 32.f;

        float fieldWidth = ImClamp(innerWidth * 0.64f, 220.f, innerWidth - 20.f);
        float fieldX = ImMax(origin.x + innerWidth - fieldWidth, origin.x + 90.f);
        const float fieldY = origin.y + (rowHeight - fieldHeight) * 0.5f;
        const float inputW = ImMax(fieldWidth - kBtnW * 2.f, 80.f);

        ImGui::SetCursorScreenPos(origin);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 0.64f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImGui::PushID(id);

        const ImVec2 btnSize = { kBtnW, fieldHeight };

        const ImVec2 leftPos = { fieldX, fieldY };
        ImGui::SetCursorScreenPos(leftPos);
        ImGui::InvisibleButton("##left", btnSize);
        const bool leftHov = ImGui::IsItemHovered(), leftHeld = ImGui::IsItemActive();
        leftTriggered = ImGui::IsItemClicked();

        const ImVec2 inputPos = { fieldX + kBtnW, fieldY };
        ImGui::SetCursorScreenPos(inputPos);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 10.f, (fieldHeight - fontSize) * 0.5f });
        ImGui::PushStyleColor(ImGuiCol_FrameBg, {});
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, {});
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, {});
        ImGui::PushItemWidth(inputW);
        ImGui::InputText("##path", buffer, bufferSize);
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);

        const bool inputHov = ImGui::IsItemHovered(), inputAct = ImGui::IsItemActive();

        const ImVec2 rightPos = { inputPos.x + inputW, fieldY };
        ImGui::SetCursorScreenPos(rightPos);
        ImGui::InvisibleButton("##apply", btnSize);
        const bool rightHov = ImGui::IsItemHovered(), rightHeld = ImGui::IsItemActive();
        applyTriggered = ImGui::IsItemClicked();

        if (leftHov || rightHov) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        const bool anyHov = inputHov || leftHov || rightHov;
        const bool anyHeld = inputAct || leftHeld || rightHeld;

        const auto frame = GuiColorFrame();
        const auto accent = GuiColorAccent();

        auto baseFill = ImLerp(frame, ImVec4(0, 0, 0, frame.w), 0.25f);
        auto hoverFill = ImLerp(baseFill, accent, 0.25f);
        auto activeFill = ImVec4(accent.x, accent.y, accent.z, 0.65f);

        auto fill = baseFill;
        if (anyHov)  fill = ImLerp(fill, hoverFill, 0.8f);
        if (anyHeld) fill = ImLerp(fill, activeFill, 0.65f);

        auto borderBase = ImLerp(frame, accent, 0.2f);
        auto borderAccent = ImVec4(accent.x, accent.y, accent.z, 0.85f);
        auto borderCol = anyHeld ? ImLerp(borderBase, borderAccent, 0.6f)
            : anyHov ? ImLerp(borderBase, borderAccent, 0.3f)
            : borderBase;

        const ImVec2 combMin = leftPos;
        const ImVec2 combMax = { rightPos.x + btnSize.x, rightPos.y + btnSize.y };
        const float rounding = fieldHeight * 0.5f;

        splitter.SetCurrentChannel(dl, 0);
        dl->AddRectFilled(combMin, combMax, ImGui::ColorConvertFloat4ToU32(fill), rounding);
        dl->AddRect(combMin, combMax, ImGui::ColorConvertFloat4ToU32(borderCol), rounding, 0, 1.1f);

        constexpr ImU32 kSepCol = IM_COL32(86, 86, 100, 210);
        dl->AddLine({ leftPos.x + kBtnW, leftPos.y + 6.f }, { leftPos.x + kBtnW, leftPos.y + btnSize.y - 6.f }, kSepCol, 1.1f);
        dl->AddLine({ rightPos.x, rightPos.y + 6.f }, { rightPos.x, rightPos.y + btnSize.y - 6.f }, kSepCol, 1.1f);

        const ImU32 glyphCol = leftHeld ? IM_COL32(255, 255, 255, 255) : leftHov ? IM_COL32(238, 238, 246, 255) : IM_COL32(216, 216, 226, 255);
        char glyph[2] = { leftGlyph, 0 };
        const auto gSz = ImGui::CalcTextSize(glyph);
        dl->AddText({ leftPos.x + (kBtnW - gSz.x) * 0.5f, leftPos.y + (btnSize.y - gSz.y) * 0.5f - 0.5f }, glyphCol, glyph);

        const ImU32 checkCol = rightHeld ? IM_COL32(255, 255, 255, 255) : rightHov ? IM_COL32(238, 238, 246, 255) : IM_COL32(216, 216, 226, 255);
        dl->AddLine(
            { rightPos.x + kBtnW * 0.28f, rightPos.y + btnSize.y * 0.55f },
            { rightPos.x + kBtnW * 0.42f, rightPos.y + btnSize.y * 0.70f }, checkCol, 2.f);
        dl->AddLine(
            { rightPos.x + kBtnW * 0.42f, rightPos.y + btnSize.y * 0.70f },
            { rightPos.x + kBtnW * 0.72f, rightPos.y + btnSize.y * 0.30f }, checkCol, 2.f);

        splitter.Merge(dl);
        ImGui::PopID();

        ImGui::SetCursorScreenPos(origin);
        ImGui::Dummy({ innerWidth, rowHeight });
        return rowHeight;
    }


    UPDATE_HOOK(ProfileChanger_Update)
    {
        if (!self || !self->Active() || !g_profilePage || !profileMenuVisible) return;

        std::vector<ProfileJob> jobs;
        {
            std::lock_guard lock(g_profileMutex);
            if (g_profileJobs.empty()) return;
            jobs.swap(g_profileJobs);
        }

        for (auto& j : jobs)
        {
            switch (j.type)
            {
            case ProfileJob::GetText:     ProfileGetTextUnity(j.field, j.outBuffer, j.outSize); break;
            case ProfileJob::ApplyText:   ProfileApplyTextUnity(j.field, j.payload); break;
            case ProfileJob::ApplyTexture: ProfileApplyTextureUnity(j.field, j.payload); break;
            case ProfileJob::DumpTexture: ProfileDumpTextureUnity(j.field, j.payload); break;
            case ProfileJob::ApplyColor:  ProfileApplyColorUnity(j.field, j.color); break;
            }
        }
    }

}

void* (*MonoInLevelPlayerProfilePageV3_get_logoutButton_Orig)(void*) = nullptr;
void* MonoInLevelPlayerProfilePageV3_get_logoutButton_Hook(void* __this)
{
    KeepCamera_OnProfileMenuOpen();
    g_profilePage = __this;
    profileMenuVisible = true;
    if (self && self->Active())
        QueueProfileTextReapplyIfFilled();
    return MonoInLevelPlayerProfilePageV3_get_logoutButton_Orig(__this);
}

void* (*MonoInLevelPlayerProfilePageV3_get_logoutButtonMask_Orig)(void*) = nullptr;
void* MonoInLevelPlayerProfilePageV3_get_logoutButtonMask_Hook(void* __this)
{
    KeepCamera_OnProfileMenuClose();
    profileMenuVisible = false;
    g_profilePage = nullptr;
    {
        std::lock_guard lock(g_profileMutex);
        profileTextRestoreQueued = false;
        g_profileJobs.clear();
    }
    return MonoInLevelPlayerProfilePageV3_get_logoutButtonMask_Orig(__this);
}

float ProfileChangerFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 16.f;

    ImGui::SetCursorScreenPos({ x, y });
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 0.65f));
    ImGui::TextUnformatted(XS("Open profile to show the side panel."));
    ImGui::PopStyleColor();
    y += ImGui::GetFontSize() + 10.f;

    return (y - detailStart.y) + 12.f;
}

void ProfileChangerFeature::OnDraw()
{
    if (!self || !self->Active()) return;
    if (!profileMenuVisible) { profileOverlayExpanded = false; return; }

    const auto& io = ImGui::GetIO();
    constexpr float kPanelW = 360.f;
    constexpr float kCollapsedW = 48.f;
    constexpr float kPanelH = 520.f;

    static float slideT = 0.f;
    const float targetX = io.DisplaySize.x - kPanelW;
    const float y = io.DisplaySize.y * 0.25f;

    const ImVec2 size = { kPanelW, kPanelH };
    const float offsetX = (1.f - slideT) * (kPanelW - kCollapsedW);
    const ImVec2 pos = { targetX + offsetX, y };

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    constexpr auto kFlags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);

    if (ImGui::Begin(XS("ProfileOverlay"), nullptr, kFlags))
    {
        auto* dl = ImGui::GetWindowDrawList();
        const auto& style = ImGui::GetStyle();
        const auto winPos = ImGui::GetWindowPos();
        const auto winSize = ImGui::GetWindowSize();

        constexpr float kCornerR = 10.f;
#ifdef ImDrawFlags_RoundCornersLeft
        constexpr auto kCornerFlags = ImDrawFlags_RoundCornersLeft;
#else
        constexpr auto kCornerFlags = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBottomLeft;
#endif
        dl->AddRectFilled(winPos, { winPos.x + winSize.x, winPos.y + winSize.y },
            ImGui::GetColorU32(style.Colors[ImGuiCol_WindowBg]), kCornerR, kCornerFlags);
        dl->AddRect(winPos, { winPos.x + winSize.x, winPos.y + winSize.y },
            ImGui::GetColorU32(style.Colors[ImGuiCol_Border]), kCornerR, kCornerFlags, 1.f);
        ImGui::PushClipRect(winPos, { winPos.x + winSize.x, winPos.y + winSize.y }, true);

        
        const auto mouse = io.MousePos;
        const float hoverW = ImLerp(kCollapsedW, kPanelW, slideT);
        const bool hovered = mouse.x >= pos.x && mouse.x <= pos.x + hoverW
            && mouse.y >= pos.y && mouse.y <= pos.y + kPanelH;
        slideT = ImLerp(slideT, hovered ? 1.f : 0.f, ImClamp(io.DeltaTime * 12.f, 0.f, 1.f));
        profileOverlayExpanded = slideT > 0.05f;
        const bool open = slideT > 0.5f;

        
        const ImVec2 barMin = { winPos.x + 8.f, winPos.y + winSize.y * 0.5f - 14.f };
        dl->AddRectFilled(barMin, { barMin.x + 4.f, barMin.y + 28.f },
            IM_COL32(255, 255, 255, static_cast<int>(255 * (open ? 0.78f : 1.f))), 2.f);

        const float contentX = winPos.x + 38.f;
        float yy = ImGui::GetCursorScreenPos().y + 12.f;
        const float width = size.x - (contentX - winPos.x) - 24.f;

        ImGui::SetCursorScreenPos({ barMin.x + 10.f, yy });
        if (tabFont) ImGui::PushFont(tabFont);
        ImGui::TextColored({ 1, 1, 1, 0.9f }, XS("Profile Changer"));
        if (tabFont) ImGui::PopFont();
        yy += ImGui::GetFontSize() + 10.f;

        static const char* modes[] = { XS("Text"), XS("Texture"), XS("Color") };
        yy += GuiDropdownRow("profile_mode_overlay", XS("Mode"), modes,
            IM_ARRAYSIZE(modes), profileModeIndex, ImVec2(contentX, yy), width) + 10.f;

        if (profileModeIndex == 0)
        {
            for (const auto& tf : kTextFields)
            {
                bool get = false, apply = false;
                yy += ProfilePathRow(tf.id, tf.label, tf.buffer, tf.bufSize,
                    'G', get, apply, ImVec2(contentX, yy), width);
                if (get)   PushJob({ ProfileJob::GetText,   tf.field, "", tf.buffer, tf.bufSize });
                if (apply) PushJob({ ProfileJob::ApplyText, tf.field, std::string(tf.buffer) });
                yy += 6.f;
            }
        }
        else if (profileModeIndex == 1)
        {
            struct TexField { const char* id; const char* label; const char* field; char* buf; size_t sz; };
            const TexField texFields[] = {
                { "profile_icon_overlay", XS("Icon"),     "icon",     profileIconPath, sizeof(profileIconPath) },
                { "profile_bg_overlay",   XS("Namecard"), "namecard", profileBgPath,   sizeof(profileBgPath)   },
            };

            for (const auto& tf : texFields)
            {
                bool dump = false, apply = false;
                yy += ProfilePathRow(tf.id, tf.label, tf.buf, tf.sz,
                    'D', dump, apply, ImVec2(contentX, yy), width);
                if (dump)  PushJob({ ProfileJob::DumpTexture,  tf.field, std::string(tf.buf) });
                if (apply) PushJob({ ProfileJob::ApplyTexture, tf.field, std::string(tf.buf) });
                yy += 6.f;
            }
        }
        else
        {
            const auto prev = profileColorName;
            const float rowH = GuiColorButtonRow("profile_color_name_overlay", XS("Slider"),
                profileColorName, ImVec2(contentX, yy), width);
            if (std::memcmp(&prev, &profileColorName, sizeof(ImVec4)) != 0)
                PushJob({ ProfileJob::ApplyColor, "Slider", "", nullptr, 0, profileColorName });
            yy += rowH + 6.f;
        }

        ImGui::SetCursorScreenPos(winPos);
        ImGui::Dummy({ winSize.x, yy - winPos.y });
        ImGui::PopClipRect();
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
}

void ProfileChangerFeature::OnShutdown()
{
    profileMenuVisible = false;
    profileOverlayExpanded = false;
    g_profilePage = nullptr;
    {
        std::lock_guard lock(g_profileMutex);
        profileTextRestoreQueued = false;
        g_profileJobs.clear();
    }
    SetEnabled(false);
}

bool IsProfileMenuVisible()
{
    return profileMenuVisible;
}