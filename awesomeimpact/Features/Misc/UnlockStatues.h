#pragma once

#include "../FeatureBase.h"

class UnlockStatuesFeature final : public Feature
{
public:
    UnlockStatuesFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
    float GetRange() const { return range_; }
	void OnEnable() override;

private:
    float range_ = 12.f;
};
