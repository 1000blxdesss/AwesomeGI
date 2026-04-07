#include "KillAura.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Logger/Logger.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"
#include "VoidAura.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <vector>

static KillAuraFeature* self{ nullptr };
KillAuraFeature::KillAuraFeature()
    : Feature({ "card_killaura", XS("Kill Aura"), XS("Automatically attack nearby enemies"), XS("NONE"), FeatureTab::Combat })
{
    self = this;
}

namespace {

    void* g_lineRenderer = nullptr;
    void* g_lineObject = nullptr;
    std::optional<std::chrono::steady_clock::time_point> g_nextAttackTime;

    inline float SampleAttackDelay(float delayMin, float delayMax)
    {
        if (delayMax < delayMin) std::swap(delayMin, delayMax);
        const float t = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return delayMin + t * (delayMax - delayMin);
    }

    inline void ResetAttackTimer(){g_nextAttackTime.reset();}

    inline void ArmAttackTimer(float delayMin, float delayMax)
    {
        const auto ms = static_cast<int64_t>(SampleAttackDelay(delayMin, delayMax) * 1000.f);
        g_nextAttackTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
    }

    inline void* GetLineRenderer()
    {
        if (g_lineRenderer) return g_lineRenderer;


        if (!g_lineObject) g_lineObject = UnityUtils::FindWithTag(UnityUtils::PtrToStringAnsi((void*)XS("MainCamera")));
        if (!g_lineObject) return nullptr;

        g_lineRenderer = UnityUtils::AddComponentInternal(g_lineObject, UnityUtils::PtrToStringAnsi((void*)XS("LineRenderer")));
        if (g_lineRenderer)
        {
            const Color lineColor{ 1.0f, 0.0f, 0.0f, 1.0f };
            UnityUtils::LineRenderer_set_useWorldSpace(g_lineRenderer, true);
            UnityUtils::LineRenderer_set_startWidth(g_lineRenderer, 0.12f);
            UnityUtils::LineRenderer_set_endWidth(g_lineRenderer, 0.12f);
            UnityUtils::LineRenderer_set_startColor(g_lineRenderer, lineColor);
            UnityUtils::LineRenderer_set_endColor(g_lineRenderer, lineColor);
        }

        return g_lineRenderer;
    }

    inline void drawRadius(const Vector3& centerRel, float radius)
    {
        if (radius <= 0.01f) return;
        void* lineRenderer = GetLineRenderer();
        if (!lineRenderer) return;
        int segments = (int)(radius * 8.0f);
        if (segments < 96) segments = 96;
        if (segments > 192) segments = 192;
        const float twoPi = 6.28318531f;
        const float step = twoPi / (float)segments;
        float cosStep = cosf(step);
        float sinStep = sinf(step);
        float cosA = 1.0f;
        float sinA = 0.0f;

        static bool groundLayerInit = false;
        static int groundLayer = 0;
        if (!groundLayerInit)
        {
            // фильтр мусора
            int baseGround = UnityUtils::Miscs_GetSceneGroundLayerMaskWithoutTemp();
            if (baseGround == 0) baseGround = UnityUtils::Miscs_GetSceneGroundLayerMask();
            int dynamicBarrier = UnityUtils::Miscs_GetDynamicBarrierLayerMask();
            int extraSurface = UnityUtils::Miscs_GetExtraSurfaceLayerMask();
            int filtered = baseGround;
            if (dynamicBarrier != 0)
                filtered &= ~dynamicBarrier;
            if (extraSurface != 0)
                filtered &= ~extraSurface;
            groundLayer = filtered != 0 ? filtered : baseGround;
            groundLayerInit = true;
        }

        // какие-то траблы с ванильным рейкастом
        const float rayStartHeight = centerRel.y; //- 50.0f;
        const float rayDetectLength = 100.0f;
        const float maxHeightAbovePlayer = 5.0f;

        static std::vector<Vector3> points;
        points.clear();
        points.reserve(segments + 1);

        for (int i = 0; i <= segments; ++i)
        {
            float dx = cosA * radius;
            float dz = sinA * radius;
            float x = centerRel.x + dx;
            float z = centerRel.z + dz;
            float y = centerRel.y;

            if (groundLayer != 0)
            {

                float detectedY = UnityUtils::Miscs_CalcCurrentGroundHeight(x, z, centerRel.y + 50.0f, 100.0f, groundLayer);

                if (std::isfinite(detectedY))
                {

                    if (detectedY > centerRel.y + maxHeightAbovePlayer) y = centerRel.y;
                    else if (detectedY < centerRel.y - 50.0f)
                        y = centerRel.y;
                    else
                        y = detectedY;
                }
            }

            Vector3 pointRel = { x, y, z };
            points.push_back(pointRel);

            float nextCos = cosA * cosStep - sinA * sinStep;
            float nextSin = sinA * cosStep + cosA * sinStep;
            cosA = nextCos;
            sinA = nextSin;
        }

        if (points.empty())
            return;
        if (points.size() > 1)
            points.back() = points.front();
        UnityUtils::LineRenderer_set_positionCount(lineRenderer, (int)points.size());
        for (int i = 0; i < (int)points.size(); ++i) UnityUtils::LineRenderer_SetPosition(lineRenderer, i, points[i]);
    }


    inline void ClearRadiusLine()
    {
        if (g_lineRenderer)
            UnityUtils::LineRenderer_set_positionCount(g_lineRenderer, 0);
    }



    UPDATE_HOOK(KillAura)
    {
        if (!self || !self->Active())
            return;

        const auto localAvatar = OtherUtils::AvatarManager();
        if (!localAvatar)
        {
            ClearRadiusLine();
            return;
        }

        const auto center = UnityUtils::GetRelativePosition_Entity(localAvatar);
        if (center.zero())
        {
            ClearRadiusLine();
            return;
        }

        if (self->IsDrawRadius())drawRadius(center, self->GetRadius());
        else
            ClearRadiusLine();

        auto* evtMgrName = UnityUtils::PtrToStringAnsi((void*)стринги_типо::EventManager());
        const auto singletonManager = UnityUtils::get_SingletonManager();
        if (!singletonManager) return;
        const auto eventManager = UnityUtils::GetSingletonInstance(singletonManager, evtMgrName);
        if (!eventManager) return;

        bool foundTarget = false;

        OtherUtils::ForEachEntity([&](void* entity) -> bool
            {
                const auto type = *reinterpret_cast<EntityType*>(
                    reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::entityType);
                if (type != EntityType::Monster && type != EntityType::MonsterEquip)
                    return true;

                const auto entityPos = UnityUtils::GetRelativePosition_Entity(entity);
                if (entityPos.zero() || center.distance(entityPos) > self->GetRadius())
                    return true;

                auto* combatComp = OtherUtils::FindProp(
                    entity,
                    UnityUtils::GetAllLogicComponents(entity),
                    стринги_типо::LCMonsterCombat());
                if (!combatComp)
                    return true;

                auto* combatProp = *reinterpret_cast<void**>(
                    reinterpret_cast<uintptr_t>(combatComp) + offsets::MoleMole::LCBaseCombat::CombatProperty_k__BackingField);
                if (!combatProp)
                    return true;

                foundTarget = true;

                if (!g_nextAttackTime)
                {
                    ArmAttackTimer(self->GetDelayMin(), self->GetDelayMax());
                    return false;
                }
                if (std::chrono::steady_clock::now() < *g_nextAttackTime)
                    return false;

                const float maxHP = UnityUtils::SafeFloatGetValue(
                    reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(combatProp) + offsets::MoleMole::SafeFloat::maxHP));
                if (maxHP <= 0.f)
                    return true;

                const auto runtimeID = *reinterpret_cast<uint32_t*>(
                    reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::runtimeID);

                constexpr float kCompensation = 2.5f;
                const float dmg = maxHP * (self->GetPrecent() / 100.f);

                auto* evt = OtherUtils::AllocateEvtCrashFromPool();
                UnityUtils::InitEvtCrash(evt, runtimeID);

                *reinterpret_cast<float*>(reinterpret_cast<char*>(evt) + offsets::MoleMole::EvtCrash::maxHp)
                    = dmg / 0.4f + kCompensation; // компенсация погрешности округления
                *reinterpret_cast<float*>(reinterpret_cast<char*>(evt) + offsets::MoleMole::EvtCrash::velChange)
                    = 1'000'000.f + static_cast<float>(std::rand() % 1'000'001);
                *reinterpret_cast<Vector3*>(reinterpret_cast<uintptr_t>(evt) + offsets::MoleMole::EvtCrash::hitPos)
                    = UnityUtils::GetAbsolutePosition(entityPos);

                UnityUtils::FireEvent(eventManager, evt, false);
                ArmAttackTimer(self->GetDelayMin(), self->GetDelayMax());
                return false;
            });

        if (!foundTarget) ResetAttackTimer();
    }
}

float KillAuraFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiSliderRow("killaura_radius", XS("Radius"),
        &radius_, 1.f, 20.f, XS("%.0f m"), ImVec2(x, y), width) + 6.f;

    y += GuiSliderRow("killaura_precent", XS("Health (%)"),
        &precent_, 0.f, 100.f, XS("%.0f %%"), ImVec2(x, y), width) + 6.f;

    y += GuiRangeSliderRow("killaura_delay", XS("Attack Delay"),
        &attackDelayMin_, &attackDelayMax_, 0.f, 15.f, "%.0f", ImVec2(x, y), width, true) + 10.f;

    y += GuiToggleTextRow("killaura_drawradius", XS("Draw Radius"),
        XS("Enabled"), drawRadius_, ImVec2(x, y), width) + 6.f;

    y += GuiToggleTextRow("killaura_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void KillAuraFeature::OnDraw()
{
}

void KillAuraFeature::OnEnable()
{
    ResetAttackTimer();
    if (auto* va = FindFeature<VoidAuraFeature>())
        va->SetEnabled(false);
}

void KillAuraFeature::OnShutdown()
{
    ClearRadiusLine();
    g_lineRenderer = nullptr;
    g_lineObject = nullptr;
    ResetAttackTimer();
    SetEnabled(false);
}
