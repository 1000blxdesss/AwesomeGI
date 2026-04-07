#pragma once

#include "../FeatureBase.h"

class OpenChestFeature final : public Feature
{
public:
    OpenChestFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
    float GetRange() const { return range_; }

private:
    float range_ = 20.f;
    bool hold_ = false;
};
