#pragma once
#include "../FeatureBase.h"
#include <string>
#include <cstdint>
#include <mutex>
#include <vector>

struct TpEntry { float x, y, z; size_t line; };

class CustomTpFeature final : public Feature
{
public:
    CustomTpFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
    void OnUpdate() override;
    void OnDraw() override;

private:
    void Load(const std::string& rawPath);
    void Step(int dir);
    void ArmAutoTp();
    void AutoTick();
    void ResetAutoTpState();

    char path_[260]{};
    int segment_ = 0;
    std::vector<TpEntry> entries_;
    size_t idx_ = 0;
    std::string status_;
    float statusT_ = 0.f;
    bool overlayInfEnabled_ = false;

    bool     autoTp_ = false;
    float    autoTpDelay_ = 3.f;
    float    autoTpDelayActive_ = 0.f;
    float    autoTpTimer_ = 0.f;
    bool     autoTpWaiting_ = false;
    uint64_t autoTpNextMs_ = 0;
    bool     autoTpInFlight_ = false;

    bool        loadRequested_ = false;
    std::string pendingPath_;
    std::mutex  dataMutex_;
};