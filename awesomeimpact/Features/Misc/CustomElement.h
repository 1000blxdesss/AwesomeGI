#pragma once

#include "../FeatureBase.h"

class CustomElementFeature final : public Feature
{
public:
    CustomElementFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;

    int GetElementValue() const;
    float GetAuraDurability() const { return auraDurability_; }
    bool ShouldOverrideDurability() const { return overrideDurability_; }
    bool ShouldSkipSelf() const { return skipSelf_; }

private:
    int elementIndex_ = 1;
    float auraDurability_ = 2.0f;
    bool overrideDurability_ = true;
    bool skipSelf_ = true;
    bool hold_ = false;
};

void CustomElement_OnFireBeingHit(uint32_t attackeeRuntimeID, void* attackResult);
