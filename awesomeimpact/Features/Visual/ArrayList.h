#pragma once

#include "../FeatureBase.h"

class ArrayListFeature final : public Feature
{
public:
    ArrayListFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
	void OnDraw() override;
	void OnShutdown() override;

private:
    bool hold_ = false;
};
