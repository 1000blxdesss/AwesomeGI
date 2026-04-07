#pragma once

#include "../FeatureBase.h"

class AirJumpFeature final : public Feature
{
public:
    AirJumpFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
	void OnShutdown() override;
	float GetExtraJumps() const { return extraJumps_; }
	bool GetNoJumpDelay() const { return noJumpDelay_; }
	bool GetBhopStyle() const { return bhopStyle_; }

private:
    float extraJumps_ = 1.f;
    bool noJumpDelay_ = false;
    bool bhopStyle_ = false;
	bool hold_ = false;
};
