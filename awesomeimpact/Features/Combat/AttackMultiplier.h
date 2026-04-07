#pragma once

#include "../FeatureBase.h"

class AttackMultiplierFeature final : public Feature
{
public:
    AttackMultiplierFeature();
    void OnShutdown() override;
    float OnGUI(const ImVec2& detailStart, float width) override;

    float GetMultiplier() const { return multiplier_; }

private:
    float multiplier_ = 10.f;
    bool hold_ = false;
};

