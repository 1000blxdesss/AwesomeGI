#pragma once

#include "../FeatureBase.h"

class FriendlyFireFeature final : public Feature
{
public:
    FriendlyFireFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;

private:
    bool hold_ = false;
};
