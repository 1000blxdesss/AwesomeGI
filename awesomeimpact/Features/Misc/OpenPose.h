#pragma once

#include "../FeatureBase.h"

class OpenPoseFeature final : public Feature
{
public:
    OpenPoseFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnDraw() override;
    void OnShutdown() override;

private:
    bool hold_ = false;
    bool menuVisible_ = false;
    bool overlayExpanded_ = false;
};
