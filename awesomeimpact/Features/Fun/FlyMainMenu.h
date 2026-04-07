#pragma once
#include "../FeatureBase.h"

class FlyMainMenu final : public Feature
{
public:
	FlyMainMenu();
	float OnGUI(const ImVec2& detailStart, float width) override;
	void OnUpdate() override;
	void OnShutdown() override;

};

