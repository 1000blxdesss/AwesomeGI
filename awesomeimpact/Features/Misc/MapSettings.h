#pragma once

#include "../FeatureBase.h"

class MapSettingsFeature final : public Feature
{
public:
    MapSettingsFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
    bool GetRedZone() const { return redZone_; }

private:
    bool redZone_ = false;
};
