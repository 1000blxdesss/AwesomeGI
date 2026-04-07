#pragma once

#include "../FeatureBase.h"
#include "../../GUI/gui.h"

class LootManagerFeature final : public Feature
{
public:
    LootManagerFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
    void ExportCustomConfigValues(std::vector<std::pair<std::string, std::string>>& out) const override;
    bool ImportCustomConfigValue(const std::string& key, const std::string& value) override;
    float GetDelayMin() const { return delayMin_; }
    float GetDelayMax() const { return delayMax_; }
    bool GetAutoPickUp() const { return autoEnabled_; }
    bool IsCustomRange() const { return customRangeEnabled_; }
    float GetRadius() const {return customRange_;}


private:
    bool autoEnabled_ = true;
    float delayMin_ = 0.f;
    float delayMax_ = 5000.f;
    bool hold_ = false;
    bool customRangeEnabled_ = true;
    float customRange_ = 20.0f;
    char whitelistBuffer_[128]{};
    char blacklistBuffer_[128]{};
    std::vector<CapsuleEntry> whitelist_;
    std::vector<CapsuleEntry> blacklist_;
};
