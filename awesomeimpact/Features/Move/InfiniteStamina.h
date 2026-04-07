#pragma once

#include "../FeatureBase.h"

class InfiniteStaminaFeature final : public Feature
{
public:
    InfiniteStaminaFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
    float GetThreshold() const { return threshold_; }
    float GetRestoreValue() const { return restoreValue_; }

private:
    float threshold_ = 3.f;
    float restoreValue_ = 10000.f;
    bool staminaDive_ = true;
    bool hold_ = false;
};
