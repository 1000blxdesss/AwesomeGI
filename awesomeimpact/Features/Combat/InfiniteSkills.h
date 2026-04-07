#pragma once

#include "../FeatureBase.h"

class InfiniteSkillsFeature final : public Feature
{
public:
    InfiniteSkillsFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;

    bool GetInfE()  const { return infSkillE_; }
    bool GetInfQ()  const { return infSkillQ_; }

private:
    bool infSkillQ_ = true;
    bool infSkillE_ = true;
    bool hold_ = false;
};
