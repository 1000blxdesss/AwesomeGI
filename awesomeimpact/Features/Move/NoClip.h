#pragma once

#include "../FeatureBase.h"

class NoClipFeature final : public Feature
{
public:
    NoClipFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
    
    float GetSpeed() const { return speed_; }
    int GetModeIndex() const { return modeIndex_; }

private:
    int modeIndex_ = 0;
    float speed_ = 10.f;
    bool hold_ = false;
};
