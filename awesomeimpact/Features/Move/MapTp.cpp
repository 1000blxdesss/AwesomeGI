#include "MapTp.h"
#include "../../GUI/gui.h"
#include "../../Offsets.h"
#include "../../globals.h"
#include "../../Utils/test02.h"

static MapTpFeature* self{ nullptr };
MapTpFeature::MapTpFeature()
    : Feature({ "card_map_tp", XS("Map TP"), XS("Teleport via map markers"), XS("NONE"), FeatureTab::Move })
{
    self = this;
}

namespace {

    inline Vector2 ScreenToMapCoord(void* pageMono, void* context, Vector2 screenPos, void* uiCamera)
    {
        void* mapBg = UnityUtils::MonoInLevelMapPage_get_mapBackground(pageMono);

        Vector2 localPos{};
        if (!UnityUtils::ScreenPointToLocalPointInRectangle(mapBg, screenPos, uiCamera, &localPos))
            return { NAN, NAN };

        const auto mapRect = UnityUtils::MonoInLevelMapPage_get_mapRect(pageMono);
        const auto viewRect = *reinterpret_cast<Rect*>(
            reinterpret_cast<uintptr_t>(context) + offsets::MoleMole::InLevelMapPageContext::_mapViewRect);

        return {
            ((localPos.x - mapRect.x) / mapRect.width) * viewRect.width + viewRect.x,
            ((localPos.y - mapRect.y) / mapRect.height) * viewRect.height + viewRect.y
        };
    }

    inline void TeleportToWorldPos(Vector3 worldPos)
    {
        const auto relPos = UnityUtils::GetRelativePosition(worldPos);
        const auto groundHeight = UnityUtils::CalcCurrentGroundHeight(relPos.x, relPos.z);

        UnityUtils::SetAvatarPos({
            worldPos.x,
            groundHeight > 0.f ? groundHeight + 5.f : 300.f,
            worldPos.z
            });

        if (const auto avatar = OtherUtils::AvatarManager())
            UnityUtils::ActorUtils_SyncEntityPos(avatar, 3, 0);
    }

}

void (*OnMapClicked_Orig)(void*, Vector2) = nullptr;
void OnMapClicked_Hook(void* context, Vector2 screenPos)
{
    if (!self || !self->Active())
        return OnMapClicked_Orig(context, screenPos);

    const auto pageMono = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(context) + offsets::MoleMole::InLevelMapPageContext::_pageMono);

    auto* uiManagerName = UnityUtils::PtrToStringAnsi((void*)стринги_типо::UIManager());

    const auto singletonMgr = UnityUtils::get_SingletonManager();
    const auto uiManager = UnityUtils::GetSingletonInstance(singletonMgr, uiManagerName);
    const auto uiCamera = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(uiManager) + offsets::MoleMole::UIManager::_uiCamera);

    const auto mapPos = ScreenToMapCoord(pageMono, context, screenPos, uiCamera);
    if (std::isnan(mapPos.x)) return OnMapClicked_Orig(context, screenPos);

    TeleportToWorldPos(UnityUtils::GenWorldPos(mapPos));
    return OnMapClicked_Orig(context, screenPos);
}
//0xD16A160, 0xD16B7D0, 0xD171990, 0xD172AA0, 0xD174660, 0xD175FB0,
//0xD177520, 0xD179590, 0xD183970, 0xD184990, 0xD184CF0, 0xD186F30,
//0xD187D60, 0xD189FF0, 0xD18AF30, 0xD18BA00, 0xD18C5C0, 0xD18CFA0,
//0xD18DB00, 0xD18EC70, 0xD18F950, 0xD190130, 0xD191840, 0xD1918E0,
//0xD191DD0, 0xD1930E0
//#6 - tp,dngns, artery etc (0xd177520)
//#22 - commonMarks
//#23 - quests (0xd1918e0)
void (*OnMarkClicked_Orig)(void*, void*) = nullptr;
void OnMarkClicked_Hook(void* context, void* monoMapMark)
{
    if (!self || !self->Active() || !self->GetAnyMark())
        return OnMarkClicked_Orig(context, monoMapMark);

    const auto generalMarkData = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(monoMapMark) + offsets::MoleMole::MonoMapMark::k__BackingField);
    const auto* mark = reinterpret_cast<GeneralMarkData*>(generalMarkData);

    UnityUtils::SetAvatarPos(mark->originPosition);
    return OnMarkClicked_Orig(context, monoMapMark);
}

float MapTpFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiToggleTextRow("map_tp_mark", XS("Any Mark"),
        XS("Enabled"), anyMark_, ImVec2(x, y), width) + 6.f;

    y += GuiToggleTextRow("map_tp_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void MapTpFeature::OnShutdown()
{
    SetEnabled(false);
}
