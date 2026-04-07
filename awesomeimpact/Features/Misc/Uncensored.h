#pragma once

#include "../FeatureBase.h"

class UncensoredFeature final : public Feature
{
public:
    UncensoredFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;

private:
    bool hold_ = false;
};
