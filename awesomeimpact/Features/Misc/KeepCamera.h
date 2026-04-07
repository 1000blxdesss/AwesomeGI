#pragma once

#include "../FeatureBase.h"

class KeepCameraFeature final : public Feature
{
public:
    KeepCameraFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
};

void KeepCamera_OnFlushPre(void* outputCamera);
void KeepCamera_OnFlushPost(void* outputCamera);
void KeepCamera_OnProfileMenuOpen();
void KeepCamera_OnProfileMenuClose();
