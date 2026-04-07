#pragma once

#include "../FeatureBase.h"

class VelocityFeature final : public Feature
{
public:
    VelocityFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
	void OnShutdown() override;

    float GetGlobalScale() const { return globalScale_; }
    float GetDetailX() const { return detailX_; }
    float GetDetailY() const { return detailY_; }
    float GetDetailZ() const { return detailZ_; }
    int GetModeIndex() const { return modeIndex_; }

private:
    int modeIndex_ = 0;
    float detailX_ = 1.f;
    float detailY_ = 1.f;
    float detailZ_ = 1.f;
    float globalScale_ = 1.f;
    bool hold_ = false;
};
