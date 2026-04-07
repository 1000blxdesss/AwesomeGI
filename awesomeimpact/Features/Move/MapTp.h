#pragma once

#include "../FeatureBase.h"

class MapTpFeature final : public Feature
{
public:
    MapTpFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
	void OnShutdown() override;
	bool GetAnyMark() const { return anyMark_; }

private:
    bool hold_ = false;
	bool anyMark_ = true;
};
