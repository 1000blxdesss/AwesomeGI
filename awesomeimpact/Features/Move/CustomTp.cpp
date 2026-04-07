#include "CustomTp.h"
#include "../../GUI/gui.h"
#include "../../Utils/HookRegistry.h"
#include "../../globals.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <locale>
#include <cctype>
#include <cmath>
#include <chrono>

static CustomTpFeature* self{ nullptr };
CustomTpFeature::CustomTpFeature()
    : Feature({ "card_custom_tp", XS("Custom TP"), XS("Teleport by list"), XS("NONE"), FeatureTab::Move })
{
    SetHeaderSplitBadge({ &segment_, &overlayInfEnabled_, XS("<"), XS(">") });
    BindConfigString("custom_tp_path", path_, IM_ARRAYSIZE(path_));
    BindConfigBool("custom_overlay_inf", &overlayInfEnabled_);
    BindConfigBool("custom_tp_auto", &autoTp_);
    BindConfigFloat("custom_tp_auto_delay", &autoTpDelay_);
    self = this;
}
namespace {

    inline uint64_t NowMs()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    inline std::string TrimQuotes(const std::string& s)
    {
        size_t start = 0, end = s.size();
        while (start < end && (s[start] == ' ' || s[start] == '"' || s[start] == '\'')) ++start;
        while (end > start && (s[end - 1] == ' ' || s[end - 1] == '"' || s[end - 1] == '\'')) --end;
        return s.substr(start, end - start);
    }

    inline void DrawSeparator(ImDrawList* dl, float width)
    {
        const auto pos = ImGui::GetCursorScreenPos();
        dl->AddLine(
            { pos.x, pos.y + 4.f },
            { pos.x + width, pos.y + 4.f },
            IM_COL32(90, 90, 100, 180), 1.f);
        ImGui::Dummy({ 0.f, 8.f });
    }

    inline void DrawActiveBorder()
    {
        if (!ImGui::IsItemActive()) return;
        auto* dl = ImGui::GetWindowDrawList();
        dl->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
            IM_COL32(255, 255, 255, 180), 4.f, 0, 1.f);
    }

}



void CustomTpFeature::Load(const std::string& rawPath)
{
    const auto cleanPath = TrimQuotes(rawPath);

    std::ifstream f(cleanPath);
    if (!f.is_open())
    {
        std::lock_guard lock(dataMutex_);
        status_ = XS("Can't open file");
        statusT_ = 3.f;
        return;
    }

    std::vector<TpEntry> parsed;
    std::string line;
    size_t lineNum = 0;

    while (std::getline(f, line))
    {
        ++lineNum;

        std::string nums;
        nums.reserve(line.size());
        for (const char c : line)
        {
            if (c == ';' || c == ',')                               nums += ' ';
            else if (c == '-' || c == '+' || c == '.' || (c >= '0' && c <= '9')) nums += c;
            else if (std::isspace(static_cast<unsigned char>(c)))   nums += ' ';
        }

        float x, y, z;
        std::istringstream iss(nums);
        iss.imbue(std::locale::classic());

        if ((iss >> x >> y >> z) && std::isfinite(x) && std::isfinite(y) && std::isfinite(z))
            parsed.push_back({ x, y, z, lineNum });
    }

    char buf[32];
    _snprintf_s(buf, 32, _TRUNCATE, XS("Loaded %zu"), parsed.size());

    std::lock_guard lock(dataMutex_);
    entries_ = std::move(parsed);
    idx_ = 0;
    status_ = buf;
    statusT_ = 2.f;
    ResetAutoTpState();
}

void CustomTpFeature::Step(int dir)
{
    TpEntry e{};
    bool autoMode = false;
    {
        std::lock_guard lock(dataMutex_);
        if (entries_.empty()) return;
        idx_ = (idx_ + dir + entries_.size()) % entries_.size();
        e = entries_[idx_];
        autoMode = autoTp_;
    }

    RunOnUnityThread([e]() {
        UnityUtils::SetAvatarPos({ e.x, e.y, e.z });
        if (const auto localAvatar = OtherUtils::AvatarManager())
            UnityUtils::ActorUtils_SyncEntityPos(localAvatar, 3, 0);
        });

    if (autoMode)
        ArmAutoTp();
}

void CustomTpFeature::ArmAutoTp()
{
    std::lock_guard lock(dataMutex_);
    if (!autoTp_ || autoTpDelay_ <= 0.f)
    {
        ResetAutoTpState();
        return;
    }

    autoTpDelayActive_ = autoTpDelay_;
    autoTpNextMs_ = NowMs() + static_cast<uint64_t>(autoTpDelay_ * 1000.f);
    autoTpTimer_ = autoTpDelay_;
    autoTpWaiting_ = true;
    autoTpInFlight_ = false;
}

void CustomTpFeature::AutoTick()
{
    bool doStep = false;
    {
        std::lock_guard lock(dataMutex_);
        if (!autoTp_ || entries_.empty())
        {
            ResetAutoTpState();
            return;
        }

        if (idx_ + 1 >= entries_.size())
        {
            ResetAutoTpState();
            return;
        }

        if (autoTpDelay_ < 0.5f)
            autoTpDelay_ = 0.5f;

        const auto now = NowMs();

        if (autoTpNextMs_ == 0)
        {
            autoTpDelayActive_ = autoTpDelay_;
            autoTpNextMs_ = now + static_cast<uint64_t>(autoTpDelay_ * 1000.f);
            autoTpTimer_ = autoTpDelay_;
            autoTpWaiting_ = true;
        }

        if (now >= autoTpNextMs_)
        {
            autoTpNextMs_ = 0;
            autoTpTimer_ = 0.f;
            autoTpWaiting_ = false;
            doStep = true;
        }
        else
        {
            autoTpWaiting_ = true;
            autoTpTimer_ = static_cast<float>(autoTpNextMs_ - now) / 1000.f;
        }
    }

    if (doStep)
        Step(1);
}

float CustomTpFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    bool confirmed = false;
    y += GuiInputConfirmRow("custom_tp_path", XS("Path"),
        path_, IM_ARRAYSIZE(path_), confirmed, ImVec2(x, y), width) + 10.f;

    y += GuiToggleTextRow("custom_overlay_inf", XS("Overlay Inf."),
        XS("Enabled"), overlayInfEnabled_, ImVec2(x, y), width) + 6.f;

    segment_ = overlayInfEnabled_ ? 1 : 0;

    if (confirmed)
    {
        std::lock_guard lock(dataMutex_);
        pendingPath_ = path_;
        loadRequested_ = true;
        status_ = XS("Loading...");
        statusT_ = 3.f;
    }

    return (y - detailStart.y) + 12.f;
}

void CustomTpFeature::OnUpdate()
{
    std::string pathToLoad;
    {
        std::lock_guard lock(dataMutex_);
        if (loadRequested_)
        {
            loadRequested_ = false;
            pathToLoad = pendingPath_;
        }
    }

    if (!pathToLoad.empty())
        Load(pathToLoad);

    if (!self->Active())
        return;

    {
        std::lock_guard lock(dataMutex_);
        if (entries_.empty()) return;
    }

    if (GetAsyncKeyState(VK_LEFT) & 1)  Step(-1);
    if (GetAsyncKeyState(VK_RIGHT) & 1) Step(1);
}

void CustomTpFeature::OnDraw()
{
    {
        std::lock_guard lock(dataMutex_);
        if (statusT_ > 0.f)
        {
            statusT_ -= ImGui::GetIO().DeltaTime;
            if (statusT_ <= 0.f)
                status_.clear();
        }
    }

    if (!self->Active()) return;
    AutoTick();
    if (!overlayInfEnabled_) return;


    std::string statusSnap;
    bool    autoSnap = false;
    bool    autoWaitingSnap = false;
    float   autoDelaySnap = 0.f;
    float   autoDelayActSnap = 0.f;
    float   autoTimerSnap = 0.f;
    bool    hasEntry = false;
    size_t  idxSnap = 0;
    size_t  totalSnap = 0;
    TpEntry currentSnap{};
    {
        std::lock_guard lock(dataMutex_);
        statusSnap = status_;
        autoSnap = autoTp_;
        autoWaitingSnap = autoTpWaiting_;
        autoDelaySnap = autoTpDelay_;
        autoDelayActSnap = autoTpDelayActive_;
        autoTimerSnap = autoTpTimer_;
        hasEntry = !entries_.empty();
        if (hasEntry)
        {
            idxSnap = idx_;
            totalSnap = entries_.size();
            currentSnap = entries_[idx_];
        }
    }


    ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.f,  0.f,  0.f,  .9f });
    ImGui::PushStyleColor(ImGuiCol_TitleBg, { .25f, .25f, .25f, .95f });
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, { .25f, .25f, .25f, .95f });
    ImGui::PushStyleColor(ImGuiCol_Button, { .15f, .15f, .15f, 1.f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { .20f, .20f, .20f, 1.f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { .15f, .15f, .15f, 1.f });
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);

    constexpr auto kWindowFlags =
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

    ImGui::SetNextWindowPos({ 20, 20 }, ImGuiCond_FirstUseEver);

    if (ImGui::Begin(XS(" "), nullptr, kWindowFlags))
    {
        auto* dl = ImGui::GetWindowDrawList();


        if (hasEntry)
        {
            ImGui::Text("(%.2f, %.2f, %.2f)", currentSnap.x, currentSnap.y, currentSnap.z);
            ImGui::TextDisabled("%zu / %zu", idxSnap + 1, totalSnap);
        }
        else
        {
            ImGui::TextDisabled(XS("Empty"));
        }

        if (!statusSnap.empty())
            ImGui::TextDisabled("%s", statusSnap.c_str());

        DrawSeparator(dl, ImGui::GetContentRegionAvail().x);


        {
            const auto pos = ImGui::GetCursorScreenPos();
            constexpr float r = 7.f;
            const ImVec2 center = { pos.x + r, pos.y + r };

            ImGui::InvisibleButton("##autotp_chk", { r * 2 + 50, r * 2 });
            if (ImGui::IsItemClicked())
            {
                std::lock_guard lock(dataMutex_);
                autoTp_ = !autoTp_;
                if (!autoTp_) ResetAutoTpState();
                else          autoTpNextMs_ = 0;
                autoSnap = autoTp_;
            }

            if (autoSnap)
            {
                dl->AddCircleFilled(center, r, IM_COL32(255, 255, 255, 220));
                dl->AddCircleFilled(center, r - 3, IM_COL32(30, 30, 30, 255));
            }
            else
            {
                dl->AddCircle(center, r, IM_COL32(150, 150, 150, 200), 0, 1.5f);
            }

            const auto textSize = ImGui::CalcTextSize(XS("Auto TP"));
            dl->AddText(
                { pos.x + r * 2 + 6, pos.y + r - textSize.y * 0.5f },
                autoSnap ? IM_COL32(255, 255, 255, 230) : IM_COL32(150, 150, 150, 200),
                XS("Auto TP"));
        }

  
        if (autoSnap)
        {
            const auto pos = ImGui::GetCursorScreenPos();
            constexpr float kSliderW = 120.f;
            constexpr float kSliderH = 14.f;
            constexpr float kHandleW = 10.f;
            constexpr float kMaxDelay = 15.f;

            dl->AddRectFilled(pos, { pos.x + kSliderW, pos.y + kSliderH },
                IM_COL32(50, 50, 50, 200), 4.f);


            if (autoWaitingSnap)
            {
                const float activeDelay = (std::max)(autoDelayActSnap > 0.f ? autoDelayActSnap : autoDelaySnap, 0.001f);
                const float progress = std::clamp(1.f - (autoTimerSnap / activeDelay), 0.f, 1.f);
                const float fillW = std::clamp(activeDelay / kMaxDelay, 0.f, 1.f) * (kSliderW - kHandleW);
                const float progressW = progress * fillW;

                if (progressW > 0.f)
                {
                    ImGui::PushClipRect(pos, { pos.x + progressW, pos.y + kSliderH }, true);
                    dl->AddRectFilled(pos, { pos.x + progressW, pos.y + kSliderH },
                        IM_COL32(100, 100, 100, 150), 4.f);
                    ImGui::PopClipRect();
                }
            }


            const float t = autoDelaySnap / kMaxDelay;
            const float handleX = pos.x + t * (kSliderW - kHandleW);
            dl->AddRectFilled({ handleX, pos.y }, { handleX + kHandleW, pos.y + kSliderH },
                IM_COL32(200, 200, 200, 230), 3.f);

            ImGui::InvisibleButton("##autotp_slider", { kSliderW, kSliderH });
            if (ImGui::IsItemActive())
            {
                const float newVal = std::clamp(
                    (ImGui::GetIO().MousePos.x - pos.x) / (kSliderW - kHandleW) * kMaxDelay,
                    0.f, kMaxDelay);
                std::lock_guard lock(dataMutex_);
                autoTpDelay_ = newVal;
                autoDelaySnap = newVal;
            }

            char buf[16];
            _snprintf_s(buf, 16, _TRUNCATE, "%.1fs", autoDelaySnap);
            const auto txtSize = ImGui::CalcTextSize(buf);
            dl->AddText(
                { pos.x + kSliderW + 6, pos.y + (kSliderH - txtSize.y) * 0.5f },
                IM_COL32(180, 180, 180, 220), buf);

            ImGui::Dummy({ 0.f, 4.f });
        }


        if (ImGui::Button(XS("L"), { 38, 0 })) Step(-1);
        DrawActiveBorder();
        ImGui::SameLine(0, 6);
        if (ImGui::Button(XS("U"), { 38, 0 }))
        {
            std::lock_guard lock(dataMutex_);
            pendingPath_ = path_;
            loadRequested_ = true;
            status_ = XS("Loading...");
            statusT_ = 3.f;
        }
        DrawActiveBorder();
        ImGui::SameLine(0, 6);
        if (ImGui::Button(XS("R"), { 38, 0 })) Step(1);
        DrawActiveBorder();
    }
    ImGui::End();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(6);
}

void CustomTpFeature::OnShutdown()
{
    std::lock_guard lock(dataMutex_);
    loadRequested_ = false;
    pendingPath_.clear();
    ResetAutoTpState();
    SetEnabled(false);
}

void CustomTpFeature::ResetAutoTpState()
{
    autoTp_ = false;
    autoTpNextMs_ = 0;
    autoTpTimer_ = 0.f;
    autoTpWaiting_ = false;
    autoTpInFlight_ = false;
    autoTpDelayActive_ = 0.f;
}