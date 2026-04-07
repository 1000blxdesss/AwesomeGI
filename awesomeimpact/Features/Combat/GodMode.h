#pragma once

#include "../FeatureBase.h"

class GodModeFeature final : public Feature
{
public:
    GodModeFeature();

    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
    bool GetNoFall() const { return noFall_; }

private:
    bool hold_ = false;
    bool noFall_ = false;
};
