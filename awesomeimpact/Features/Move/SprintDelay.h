#pragma once

#include "../FeatureBase.h"

class SprintDelayFeature final : public Feature
{
public:
    SprintDelayFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
	void OnShutdown() override;

private:
    bool hold_ = false;
};
