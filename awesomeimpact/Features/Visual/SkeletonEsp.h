#pragma once

#include "../FeatureBase.h"
#include "../../GUI/gui.h"

class SkeletonEspFeature final : public Feature
{
public:
    SkeletonEspFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnDraw() override;
    void OnShutdown() override;

private:
    int targetIndex_ = 0;
    ImVec4 boneColor_ = ImVec4(0.52f, 0.78f, 0.98f, 1.f);
};
