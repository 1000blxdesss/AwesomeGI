#pragma once

#include "../FeatureBase.h"

class AutoDialogFeature final : public Feature
{
public:
    AutoDialogFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    bool AutoSelectEnabled() const { return autoSelect_; }
    bool AutoSkipEnabled() const { return autoSkip_; }
    void OnShutdown() override;

private:
    bool hold_ = false;
    bool autoSelect_ = true;
    bool autoSkip_ = true;
};

namespace AutoDialogRuntime
{
    void StartForceSkipWindow(float seconds);
    void StartForceImmediateCloseWindow(float seconds);
    bool IsForceSkipActive();
}
