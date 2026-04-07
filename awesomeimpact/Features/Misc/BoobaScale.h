#pragma once

#include "../FeatureBase.h"

class BoobaScaleFeature final : public Feature
{
public:
    BoobaScaleFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
    float GetScale() const { return scale_; }

private:
    float scale_ = 5.f;
    bool hold_ = false;
};
