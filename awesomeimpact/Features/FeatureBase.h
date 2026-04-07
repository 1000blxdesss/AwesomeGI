#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

struct ImVec2;
struct ImVec4;

enum class FeatureTab
{
    Combat,
    Move,
    Visual,
    Misc,
    Profiles
};

struct FeatureInfo
{
    const char* id;
    const char* title;
    const char* desc;
    const char* badge;
    FeatureTab tab;
};

struct HeaderSplitBadge
{
    int* value = nullptr;
    bool* linkedBool = nullptr;
    const char* leftLabel = nullptr;
    const char* rightLabel = nullptr;
};

struct CardState
{
    bool expanded = false;
    bool active = false;
    float anim = 0.f;
    float detailHeight = 120.f;
    float hover = 0.f;
    float pressGlow = 0.f;
};

enum class ConfigFieldType
{
    Bool,
    Int,
    Float,
    String,
    Color4
};

struct ConfigFieldBinding
{
    ConfigFieldType type = ConfigFieldType::Bool;
    void* ptr = nullptr;
    size_t size = 0;
};

class Feature
{
public:
    explicit Feature(FeatureInfo info) : info_(info) {}
    virtual ~Feature() = default;

    const FeatureInfo& Info() const { return info_; }
    bool Enabled() const { return enabled_; }
    bool Active() const { return active_; }
    int KeybindKey() const { return keybindKey_; }
    bool KeybindHold() const { return keybindHold_; }
    bool KeybindCapturing() const { return keybindCapturing_; }
    const std::unordered_map<std::string, ConfigFieldBinding>& ConfigBindings() const { return configBindings_; }
    const HeaderSplitBadge* HeaderSplitBadgeConfig() const { return hasHeaderSplit_ ? &headerSplit_ : nullptr; }
    std::vector<std::pair<std::string, std::string>> ExportConfigValues() const;
    void ImportConfigValue(const std::string& key, const std::string& value);
    void BindConfigBool(const char* key, bool* value);
    void BindConfigInt(const char* key, int* value);
    void BindConfigFloat(const char* key, float* value);
    void BindConfigString(const char* key, char* value, size_t capacity);
    void BindConfigColor(const char* key, ImVec4* value);

    void SetEnabled(bool enabled)
    {
        if (ignoreDisableFromShutdown_ && !enabled)
            return;
        if (enabled_ == enabled)
            return;
        enabled_ = enabled;
        if (keybindKey_ != 0 && !keybindHold_)
            keybindToggleLatched_ = enabled_;
        RefreshActiveState();
    }

    void SetKeybindKey(int key, bool suppressUntilRelease = false)
    {
        const bool wasEnabled = enabled_;
        keybindKey_ = key > 0 ? key : 0;
        keybindPrevDown_ = false;
        keybindSuppressUntilRelease_ = suppressUntilRelease;
        if (keybindKey_ == 0)
            keybindToggleLatched_ = true;
        else if (!keybindHold_)
            keybindToggleLatched_ = wasEnabled;
        RefreshActiveState();
    }

    void ClearKeybind()
    {
        SetKeybindKey(0, false);
    }

    void SetKeybindHold(bool hold)
    {
        if (keybindHold_ == hold)
            return;
        const bool wasEnabled = enabled_;
        keybindHold_ = hold;
        keybindPrevDown_ = false;
        keybindSuppressUntilRelease_ = false;
        if (!keybindHold_)
            keybindToggleLatched_ = wasEnabled;
        RefreshActiveState();
    }

    void SetKeybindCapturing(bool capturing)
    {
        keybindCapturing_ = capturing;
    }

    void Tick()
    {
        RefreshActiveState();
        if (active_)
            OnUpdate();
    }

    void SetHeaderSplitBadge(const HeaderSplitBadge& badge)
    {
        headerSplit_ = badge;
        hasHeaderSplit_ = true;
    }

    virtual void OnUpdate() {}
    virtual float OnGUI(const ImVec2& detailStart, float width) = 0;
    virtual void OnShutdown() {}
    virtual void OnDraw() {}
    virtual void OnEnable() {}
    virtual void ExportCustomConfigValues(std::vector<std::pair<std::string, std::string>>& out) const {}
    virtual bool ImportCustomConfigValue(const std::string& key, const std::string& value) { return false; }

    CardState card;

private:
    void BindConfigInternal(const char* key, ConfigFieldType type, void* ptr, size_t size);
    bool TryApplyConfigValue(const std::string& key, const std::string& value);

    bool ComputeBindGate()
    {
        if (keybindKey_ == 0)
        {
            keybindToggleLatched_ = true;
            keybindPrevDown_ = false;
            keybindSuppressUntilRelease_ = false;
            return true;
        }

        const bool down = (GetAsyncKeyState(keybindKey_) & 0x8000) != 0;
        if (keybindSuppressUntilRelease_)
        {
            if (!down)
                keybindSuppressUntilRelease_ = false;
        }

        if (keybindHold_)
        {
            keybindPrevDown_ = down;
            return down && !keybindSuppressUntilRelease_;
        }

        if (down && !keybindPrevDown_ && !keybindSuppressUntilRelease_)
        {
            keybindToggleLatched_ = !keybindToggleLatched_;
            enabled_ = keybindToggleLatched_;
        }

        keybindPrevDown_ = down;
        return true;
    }

    void RefreshActiveState()
    {
        const bool bindGate = ComputeBindGate();
        const bool nextActive = enabled_ && bindGate;
        if (nextActive == active_)
            return;
        const bool deactivatedByBindGate = active_ && !nextActive && enabled_ && !bindGate;
        active_ = nextActive;
        if (active_)
            OnEnable();
        else
        {
            const bool prevGuard = ignoreDisableFromShutdown_;
            ignoreDisableFromShutdown_ = deactivatedByBindGate;
            OnShutdown();
            ignoreDisableFromShutdown_ = prevGuard;
        }
    }

    FeatureInfo info_;
    bool enabled_ = false;
    bool active_ = false;
    HeaderSplitBadge headerSplit_{};
    bool hasHeaderSplit_ = false;
    int keybindKey_ = 0;
    bool keybindHold_ = false;
    bool keybindCapturing_ = false;
    bool keybindToggleLatched_ = true;
    bool keybindPrevDown_ = false;
    bool keybindSuppressUntilRelease_ = false;
    bool ignoreDisableFromShutdown_ = false;
    std::unordered_map<std::string, ConfigFieldBinding> configBindings_;
    std::unordered_map<std::string, std::string> pendingConfigValues_;
};

class FeatureManager
{
public:
    template <typename T, typename... Args>
    T& AddFeature(Args&&... args)
    {
        static_assert(std::is_base_of_v<Feature, T>);
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *ptr;
        features_.emplace_back(std::move(ptr));
        return ref;
    }

    std::vector<std::unique_ptr<Feature>>& All() { return features_; }

    void UpdateAll()
    {
        for (auto& f : features_)
            f->Tick();
    }

    void ShutdownAll()
    {
        for (auto& f : features_)
        {
            if (f->Enabled())
                f->SetEnabled(false);
        }
    }

    void ImGuiDrawAll()
    {
        for (auto& f : features_)
            if (f->Active())
                f->OnDraw();
    }

private:
    std::vector<std::unique_ptr<Feature>> features_;
};

FeatureManager& GetFeatureManager();
