#pragma once

#include "../FeatureBase.h"

class GuiStyleFeature final : public Feature
{
public:
    GuiStyleFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
};
