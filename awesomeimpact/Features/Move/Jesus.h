#pragma once

#include "../FeatureBase.h"

class JesusFeature final : public Feature
{
public:
    JesusFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
	void OnShutdown() override;

private:
    bool hold_ = false;
};
