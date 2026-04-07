#pragma once

#include "../FeatureBase.h"

class CameraSettingsFeature final : public Feature
{
public:
    CameraSettingsFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
	void OnShutdown() override;
	float GetFov() const { return fov_; }
	float GetDrawDistance() const { return drawDistance_; }
	bool FirstPersonEnabled() const { return firstPersonEnabled_; }

private:
    float fov_ = 60.f;
    float drawDistance_ = 500.f;
    bool firstPersonEnabled_ = false;
    bool firstPersonHold_ = true;
};
