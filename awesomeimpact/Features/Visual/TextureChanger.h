#pragma once

#include "../FeatureBase.h"
#include "../../GUI/gui.h"
#include <atomic>
#include <mutex>

class TextureChangerFeature final : public Feature
{
public:
    TextureChangerFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;

    std::vector<TextureTargetEntry> targets_;
    std::vector<TextureTargetEntry> pendingTargets_;
    std::mutex pendingMutex_;
    std::atomic<bool> pendingSwap_{ false };
    std::atomic<bool> scanQueued_{ false };
    bool scanFilled_ = false;
    bool pendingScanFilled_ = false;
    bool hold_ = false;
    std::string statusText_;
};
