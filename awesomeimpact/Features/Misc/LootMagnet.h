#pragma once

#include "../FeatureBase.h"
#include "../../GUI/gui.h"

class LootMagnetFeature final : public Feature
{
public:
    LootMagnetFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
    void ExportCustomConfigValues(std::vector<std::pair<std::string, std::string>>& out) const override;
    bool ImportCustomConfigValue(const std::string& key, const std::string& value) override;
    const std::vector<CapsuleEntry>& GetNames() {return names_;}

private:
    char buffer_[128]{};
    std::vector<CapsuleEntry> names_;
    bool hold_ = false;
};
