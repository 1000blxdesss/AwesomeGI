#pragma once

#include "../FeatureBase.h"

class SpinFeature final : public Feature
{
public:
    SpinFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
	void OnShutdown() override;
	int GetModeIndex() const { return modeIndex_; }
	float GetSpeed() const { return speed_; }

private:
    int modeIndex_ = 0;
    float speed_ = 10.f;
    bool hold_ = false;
};
