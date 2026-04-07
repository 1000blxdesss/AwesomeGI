#pragma once

#include "../FeatureBase.h"

class AutoDestroyFeature final : public Feature
{
public:
    AutoDestroyFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
    float GetRange() const { return range_; }
    bool GetHighlight() const { return highlight_; }

private:
    float range_ = 25.f;
    bool hold_ = false;
    bool highlight_ = false;
};
