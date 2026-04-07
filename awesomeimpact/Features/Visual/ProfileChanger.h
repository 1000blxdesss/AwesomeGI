#pragma once

#include "../FeatureBase.h"

class ProfileChangerFeature final : public Feature
{
public:
    ProfileChangerFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnDraw() override;
    void OnShutdown() override;
};

bool IsProfileMenuVisible();
