#include "BowTp.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

static BowTpFeature* self{ nullptr };
BowTpFeature::BowTpFeature()
    : Feature({ "bow_tp", XS("Bow TP"), XS("Follow the arrow"), XS("NONE"), FeatureTab::Move })
{
    self = this;
}

namespace {

    void* g_arrowEntity = nullptr;
    bool g_trackArrow = false;
    bool g_pendingBowTp = false;

    inline void ClearArrow()
    {
        g_arrowEntity = nullptr;
        g_pendingBowTp = false;
    }

    UPDATE_HOOK(BowTP)
    {
        if (!g_pendingBowTp || !self || !self->Active())return;

        g_pendingBowTp = false;

        auto* localAvatar = OtherUtils::AvatarManager();
        if (!localAvatar || !g_arrowEntity) return;

        if (OtherUtils::IsEntityRemoved(g_arrowEntity))
        {
            ClearArrow();
            return;
        }

        UnityUtils::SetRelativePosition(
            localAvatar,
            UnityUtils::GetRelativePosition_Entity(g_arrowEntity),
            true);
    }

}

void (*AddEntity_Orig)(void*, void*) = nullptr;
void AddEntity_Hook(void* entityManager, void* baseEntity)
{
    AddEntity_Orig(entityManager, baseEntity);

    if (!baseEntity || !((self && self->Active()) || g_trackArrow))
        return;

    if (OtherUtils::GetEntityType(baseEntity) != EntityType::Bullet)
        return;

    auto* localAvatar = OtherUtils::AvatarManager();
    if (!localAvatar)
        return;

    auto* owner = UnityUtils::GetEntityOwner(baseEntity);
    if (!owner || owner != localAvatar || OtherUtils::GetEntityType(owner) != EntityType::Avatar)
        return;

    g_arrowEntity = baseEntity;
    g_pendingBowTp = true;
}

float BowTpFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    ImGui::SetCursorScreenPos(ImVec2(x, y));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.58f));
    ImGui::TextUnformatted(XS(":3"));
    ImGui::PopStyleColor();
    y += ImGui::GetFontSize() + 12.f;

    y += GuiToggleTextRow("bow_tp_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void BowTpFeature::OnShutdown()
{
    ClearArrow();
    g_trackArrow = false;
    SetEnabled(false);
}

void BowTpFeature::SetArrowTrackEnabled(bool enabled)
{
    g_trackArrow = enabled;
    if (!g_trackArrow && !(self && self->Active()))
        ClearArrow();
}

void* BowTpFeature::GetArrowEntity()
{
    if (!g_arrowEntity) return nullptr;

    if (OtherUtils::IsEntityRemoved(g_arrowEntity))
    {
        ClearArrow();
        return nullptr;
    }

    return g_arrowEntity;
}