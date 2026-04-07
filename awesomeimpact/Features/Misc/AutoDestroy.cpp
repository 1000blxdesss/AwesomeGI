#include "AutoDestroy.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#include <cmath>
#include <cstdlib>
#include <unordered_set>

static AutoDestroyFeature* self{ nullptr };
AutoDestroyFeature::AutoDestroyFeature()
    : Feature({ "card_auto_destroy", XS("Auto Destroy"), XS("Destroy around objects"), "NONE", FeatureTab::Misc })
{
    self = this;
}

namespace {

    std::unordered_set<void*> g_glowEntities;
    void ApplyRainbowGlow(void* entity)
    {
        if (!entity) return;

        static int pidZTest = UnityUtils::Shader_PropertyToID(UnityUtils::PtrToStringAnsi((void*)XS("_ZTest")));
        static int pidZWrite = UnityUtils::Shader_PropertyToID(UnityUtils::PtrToStringAnsi((void*)XS("_ZWrite")));
        static int pidCull = UnityUtils::Shader_PropertyToID(UnityUtils::PtrToStringAnsi((void*)XS("_Cull")));

        static void* chamShader = [] {
            auto* s = UnityUtils::Shader_Find(UnityUtils::PtrToStringAnsi((void*)XS("Hidden/Internal-Colored")));
            return s ? s : UnityUtils::Shader_Find(UnityUtils::PtrToStringAnsi((void*)XS("Unlit/Color")));
            }();

        if (!chamShader) return;

        auto* tool = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::monoVisualEntityTool);
        if (!tool) return;

        auto* renderers = UnityUtils::MonoVisualEntityTool_get_renderers(tool);
        if (!renderers || renderers->max_length == 0 || renderers->max_length > 1000) return;

        const float t = static_cast<float>(ImGui::GetTime());
        const float r = std::sin(t * 2.f) * 0.5f + 0.5f;
        const float g = std::sin(t * 2.f + 2.09f) * 0.5f + 0.5f;
        const float b = std::sin(t * 2.f + 4.18f) * 0.5f + 0.5f;
        const float pulse = std::sin(t * 4.f) * 0.5f + 2.5f;

        for (uint32_t j = 0; j < renderers->max_length; ++j)
        {
            auto* renderer = renderers->vector[j];
            if (!renderer) continue;

            UnityUtils::Renderer_set_skipGPUCulling(renderer, true);
            UnityUtils::Renderer_set_viewDistanceRatio(renderer, 1e9f);

            auto* mats = UnityUtils::Renderer_get_sharedMaterials(renderer);
            if (!mats || mats->max_length == 0 || mats->max_length > 256) continue;

            for (uint32_t m = 0; m < mats->max_length; ++m)
            {
                auto* mat = mats->vector[m];
                if (!mat) continue;

                UnityUtils::Material_set_shader(mat, chamShader);
                UnityUtils::Material_set_color(mat, { r * pulse, g * pulse, b * pulse, 1.f });
                UnityUtils::Material_SetInt(mat, pidZTest, 8);
                UnityUtils::Material_SetInt(mat, pidZWrite, 0);
                UnityUtils::Material_SetInt(mat, pidCull, 0);
                UnityUtils::Material_set_renderQueue(mat, 5000);
            }
        }
    }

}

void (*ReduceModifierDurability_Orig)(void*, int, float, Nullable<float>) = nullptr;
void ReduceModifierDurability_Hook(void* lcAbilityElement, int modifierDurabilityIndex, float reduceDurability, Nullable<float> deltaTime)
{
    ReduceModifierDurability_Orig(lcAbilityElement, modifierDurabilityIndex, reduceDurability, deltaTime);

    if (!self || !self->Active()) return;
    if (!lcAbilityElement) return;

    const auto avatar = OtherUtils::AvatarManager();
    if (!avatar) return;

    const auto entity = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(lcAbilityElement) + offsets::MoleMole::BaseComponent::_entity);
    if (!entity) return;

    const auto entityType = *reinterpret_cast<EntityType*>(
        reinterpret_cast<uintptr_t>(entity) + offsets::MoleMole::BaseEntity::entityType);

    if (entityType == EntityType::Avatar || entityType == EntityType::NPC || entityType == EntityType::EnvAnimal || entityType == EntityType::Monster) return;

    const auto playerPos = UnityUtils::GetRelativePosition_Entity(avatar);
    const auto entityPos = UnityUtils::GetRelativePosition_Entity(entity);

    if (playerPos.distance(entityPos) > self->GetRange()) return;

    if (self->GetHighlight()) g_glowEntities.insert(entity);

    const float dmg = 900.f + (static_cast<float>(std::rand()) / RAND_MAX) * 100.f;
    ReduceModifierDurability_Orig(lcAbilityElement, modifierDurabilityIndex, dmg, deltaTime);
}

UPDATE_HOOK(AutoDestroy_Highlight)
{
    if (!self || !self->Active() || !self->GetHighlight())
    {
        g_glowEntities.clear();
        return;
    }

    for (auto it = g_glowEntities.begin(); it != g_glowEntities.end(); )
    {
        auto* entity = *it;
        if (!entity || OtherUtils::IsEntityRemoved(entity))
        {
            it = g_glowEntities.erase(it);
            continue;
        }

        ApplyRainbowGlow(entity);
        ++it;
    }
}

float AutoDestroyFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiSliderRow("destroy_range", XS("Range"),
        &range_, 1.f, 100.f, "%.0f", ImVec2(x, y), width, true) + 10.f;

    y += GuiToggleTextRow("destroy_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;

    y += GuiToggleTextRow("destroy_highlight", XS("Highlight"),
        XS("Enabled"), highlight_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void AutoDestroyFeature::OnShutdown()
{
    g_glowEntities.clear();
    SetEnabled(false);
}
