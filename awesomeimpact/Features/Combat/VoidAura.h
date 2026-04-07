#pragma once

#include "../FeatureBase.h"

class VoidAuraFeature final : public Feature
{
public:
    VoidAuraFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
	void OnShutdown() override;
	void OnDraw() override;
    void OnEnable() override;

	float GetRadius() const { return radius_; }

private:
    float radius_ = 4.f;
    bool hold_ = false;
    bool drawRadius_ = false;
    int modeIndex_ = 0;
};
