#pragma once
#include "../FeatureBase.h"

class InstantBowFeature final : public Feature
{
public:
	InstantBowFeature();
	float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
private:
	bool hold_ = false;
};
