#pragma once

#include "../FeatureBase.h"

class NameTagsFeature final : public Feature
{
public:
    NameTagsFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnDraw() override;
    void OnShutdown() override;
    float GetScale() const { return scale_; }
    float GetTextScale() const { return textScale_; }
    bool ShowHp() const { return showHp_; }

private:
    int targetIndex_ = 0;
    float scale_ = 1.35f;
    float textScale_ = 1.2f;
    bool showHp_ = true;
	bool showObjName_ = false;
};
