#pragma once

#include "../FeatureBase.h"
#include "../../GUI/gui.h"
#include "../../globals.h"

#include <array>

class CustomEffectFeature final : public Feature
{
public:
    CustomEffectFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
	void OnDraw() override;
    void OnEnable() override;
	void OnShutdown() override;

private:
    std::array<char, 260> effectName_{};
};
