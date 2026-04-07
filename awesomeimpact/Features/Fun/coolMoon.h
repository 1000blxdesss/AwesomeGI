#pragma once

#include "../FeatureBase.h"

class CoolMoonFeature final : public Feature
{
public:
    CoolMoonFeature();
	void OnShutdown() override;
	float OnGUI(const ImVec2& detailStart, float width) override;
	void OnEnable() override;
};

