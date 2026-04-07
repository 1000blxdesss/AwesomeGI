#pragma once

#include "../FeatureBase.h"

class SkipCutscenesFeature final : public Feature
{
public:
    SkipCutscenesFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
};
