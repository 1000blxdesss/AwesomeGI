#pragma once

#include "../FeatureBase.h"

class BetterFlyFeature final : public Feature
{
public:
    BetterFlyFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
	float GetFlySpeed() const { return flySpeed_; }
	bool GetFlyCloakUnlock() const { return flyCloakUnlock_; }
    

private:
	bool hold_ = false;
	bool flyCloakUnlock_ = false;
	float flySpeed_ = 10.f;
};
