#pragma once

#include "../FeatureBase.h"

class FpsOverlayFeature final : public Feature
{
public:
    FpsOverlayFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnDraw() override;
    void OnShutdown() override;

private:
    bool hold_ = false;
};
