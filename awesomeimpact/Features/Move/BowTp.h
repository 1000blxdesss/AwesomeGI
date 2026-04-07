#pragma once

#include "../FeatureBase.h"

class BowTpFeature final : public Feature
{
public:
    BowTpFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
	void OnShutdown() override;

    static void SetArrowTrackEnabled(bool enabled);
    static void* GetArrowEntity();

private:
    bool hold_ = false;
};
