#pragma once

#include "../FeatureBase.h"

class FpsUnlockFeature final : public Feature
{
public:
    FpsUnlockFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnEnable() override;
    void OnShutdown() override;
    float GetLimit() const { return limit_; }

private:
    float limit_ = 60.f;
    bool hold_ = false;
};

