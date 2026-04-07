#pragma once
#include "../FeatureBase.h"
#include "../../GUI/imgui.h"

#include <array>

class CombatPropFeature final : public Feature
{
public:
    CombatPropFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;

private:
    int selectedPropIndex_ = 0;
    std::array<char, 64> valueInput_{};
};
