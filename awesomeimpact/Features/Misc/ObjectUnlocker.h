#pragma once

#include "../FeatureBase.h"

class ObjectUnlockerFeature final : public Feature
{
public:
    ObjectUnlockerFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
    float GetRange() const { return rangeW_; }
    float GetElemRange() const { return rangeE_; }
    float GetDelay() const { return elemDelaySec_; }

private:
    float rangeW_ = 45.f;
    float rangeE_ = 45.f;
    float elemDelaySec_ = 0.25f;
    int modeIndex_ = 0;
};
