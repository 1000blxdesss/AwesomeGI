#pragma once
#include "../FeatureBase.h"
#include "../../GUI/gui.h"
#include "../../globals.h"

#include <deque>

class TrailFeature :
    public Feature
{
public:
    TrailFeature();
    float OnGUI(const ImVec2& detailStart, float width) override; 
    void OnDraw() override; 
    void OnShutdown() override; 

    const ImVec4& ColorValue() const { return color_; }
    float Thickness() const { return thickness_; }
    float DurationSec() const { return durationSec_; }
    bool ArrowTrailEnabled() const { return arrowTrailEnabled_; }
    int TrailModeIndex() const { return trailMode_; }

private: 
    struct TrailPoint
    {
        Vector3 world{};
        double timeStamp = 0.0;
    };

    ImVec4 color_ = ImVec4(0.45f, 0.72f, 1.0f, 1.f);
    float thickness_ = 1.8f;
    float durationSec_ = 3.0f;
    bool arrowTrailEnabled_ = false;
    int trailMode_ = 0;
    bool hold_ = false;
    float minSegmentDist_ = 0.08f;
    int subdivisions_ = 6;
    std::deque<TrailPoint> points_;
    double elapsedTime_ = 0.0;
};
