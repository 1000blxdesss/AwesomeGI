#pragma once

#include "../FeatureBase.h"
#include <string>
#include <vector>

class DefaultProfileFeature final : public Feature
{
public:
    DefaultProfileFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void RefreshProfiles();
    bool SaveProfile(const std::string& requestedName, std::string& outStatus);
    bool LoadProfile(const std::string& profileName, std::string& outStatus);
    bool DeleteProfile(const std::string& profileName, std::string& outStatus);
    void TickStatus(float deltaSeconds);
    const std::string& StatusText() const { return status_; }
    const std::vector<std::string>& ProfileNames() const { return profileNames_; }
    const char* ProfileNameBuffer() const { return profileNameInput_; }
    void SetProfileNameBuffer(const std::string& value);
    void ShowStatus(const std::string& text, float seconds);

private:
    void SetStatus(const std::string& text, float seconds);

    char profileNameInput_[128]{};
    std::vector<std::string> profileNames_;
    std::string status_;
    float statusTimer_ = 0.f;
};
