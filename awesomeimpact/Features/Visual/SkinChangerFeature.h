#pragma once

#include "../FeatureBase.h"
#include "../../GUI/gui.h"
#include <array>

class SkinChangerFeature : public Feature
{
public:
    SkinChangerFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;

    std::array<char, 16> flycloakBuf_{};
    std::array<char, 16> costumeBuf_{};
    std::string statusText_;
};
