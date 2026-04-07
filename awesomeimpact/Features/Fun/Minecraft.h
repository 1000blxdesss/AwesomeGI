#pragma once
#include <array>

#include "../FeatureBase.h"

class MinecraftFeature final : public Feature
{
public:
    MinecraftFeature();
    void OnEnable() override;
    void OnUpdate() override;
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;

    const char* GetTexturePath() const { return texturePath_.data(); }
    float GetBlockSize() const { return blockSize_; }

private:
    float blockSize_ = 1.f;
    std::array<char, 260> texturePath_{};
};
