#pragma once

#include "../FeatureBase.h"
#include "../../GUI/gui.h"

class ChamsFeature final : public Feature
{
public:
    ChamsFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;

    std::vector<CapsuleEntry> materials_;
    ImVec4 color_ = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool hold_ = false;
    bool scanFilled_ = false;
};
