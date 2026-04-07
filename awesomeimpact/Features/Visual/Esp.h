#pragma once

#include "../FeatureBase.h"
#include "../../GUI/gui.h"

class EspFeature final : public Feature
{
public:
    EspFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnDraw() override;
    void OnShutdown() override;
    void ExportCustomConfigValues(std::vector<std::pair<std::string, std::string>>& out) const override;
    bool ImportCustomConfigValue(const std::string& key, const std::string& value) override;
    bool IsSpiritsEnabled() const { return spirits_; }
    bool IsHighlightEnabled() const { return highlight_; }
    const std::vector<CapsuleEntry>& GetIncludeList() const { return includeList_; }

private:
    int modeIndex_ = 0;
    bool holdKey_ = false;
    bool filled_ = false;
    bool tracers_ = false;
    bool spirits_ = false;
    bool highlight_ = false;
    char inputBuffer_[128]{};
    std::vector<CapsuleEntry> includeList_;
};
