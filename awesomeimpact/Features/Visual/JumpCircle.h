#pragma once

#include "../FeatureBase.h"
#include "../../GUI/gui.h"

class JumpCircleFeature final : public Feature
{
public:
    JumpCircleFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
	void OnDraw() override;
	void OnShutdown() override;
    float GetMaxRadius() const { return maxRadius_; }
    float GetSpeed() const { return speed_; }
    float GetThickness() const { return thickness_; }
    const ImVec4& GetColor() const { return color_; }

private:
    ImVec4 color_ = ImVec4(1.f, 1.f, 1.f, 1.f);
    bool hold_ = false;
    float speed_ = 6.0f;
    float maxRadius_ = 4.50f;
    float thickness_ = 0.5f;
};
