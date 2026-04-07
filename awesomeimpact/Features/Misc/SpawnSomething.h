#pragma once

#include "../FeatureBase.h"

class SpawnSomethingFeature final : public Feature
{
public:
    SpawnSomethingFeature();
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;

private:
    char gadgetIdBuffer_[32]{};
    char avatarIdBuffer_[32]{};
    char enemyIdBuffer_[32]{};
    char npcIdBuffer_[32]{};
    bool gadgetPending_ = false;
    bool avatarPending_ = false;
    bool enemyPending_ = false;
    bool npcPending_ = false;
    int gadgetId_ = 0;
    int avatarId_ = 0;
    int enemyId_ = 0;
    int npcId_ = 0;
};
