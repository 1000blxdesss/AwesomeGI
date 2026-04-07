#pragma once

#include "../FeatureBase.h"
#include "../../GUI/gui.h"

class WireframeFeature final : public Feature
{
public:
    WireframeFeature();
   // void OnUpdate() override;
    float OnGUI(const ImVec2& detailStart, float width) override;

private:
    ImVec4 color_ = ImVec4(0.28f, 0.84f, 0.84f, 1.f);
    bool hold_ = false;
    bool global_ = false;
    bool aggressive_ = false;
};
