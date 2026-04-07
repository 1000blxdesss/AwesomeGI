#include "Chams.h"
#include "../../GUI/gui.h"
#include "../../GUI/imgui_internal.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#include <algorithm>
#include <string>

static ChamsFeature* self{ nullptr };
ChamsFeature::ChamsFeature()
    : Feature({ "card_chams", XS("Chams"), XS("Custom highlight materials"), "NONE", FeatureTab::Visual })
{
    self = this;
}

namespace {

    inline std::string GetMaterialName(void* mat)
    {
        auto* nameStr = UnityUtils::Object_get_name(mat);
        if (!nameStr) return {};
        return OtherUtils::Utf16ToUtf8(nameStr->chars, nameStr->length);
    }

    inline void* GetVisualTool()
    {
        const auto localAvatar = OtherUtils::AvatarManager();
        if (!localAvatar) return nullptr;

        auto* avatarGO = UnityUtils::BaseEntity_rootObject(localAvatar);
        if (!avatarGO) return nullptr;

        static Il2CppString* toolName =
            UnityUtils::PtrToStringAnsi((void*)XS("MonoVisualEntityTool"));

        return UnityUtils::GameObject_GetComponentByName(avatarGO, toolName);
    }

    template<typename Fn>
    inline void ForEachMaterial(Fn&& fn)
    {
        auto* visualTool = GetVisualTool();
        if (!visualTool) return;

        auto* renderers = UnityUtils::MonoVisualEntityTool_get_renderers(visualTool);
        if (!renderers || renderers->max_length == 0 || renderers->max_length > 1000)
            return;

        for (uint32_t i = 0; i < renderers->max_length; ++i)
        {
            auto* renderer = renderers->vector[i];
            if (!renderer) continue;

            auto* mats = UnityUtils::Renderer_get_sharedMaterials(renderer);
            if (!mats || mats->max_length == 0 || mats->max_length > 256)
                continue;

            for (uint32_t m = 0; m < mats->max_length; ++m)
            {
                auto* mat = mats->vector[m];
                if (mat) fn(mat);
            }
        }
    }

    void ApplyChams()
    {
        if (!self || !self->Active()) return;

        ForEachMaterial([](void* mat) {
            const auto name = GetMaterialName(mat);
            if (name.empty()) return;

            auto it = std::find_if(self->materials_.begin(), self->materials_.end(),
                [&](const CapsuleEntry& e) { return e.label == name; });

            const auto& col = (it != self->materials_.end()) ? it->color : self->color_;
            UnityUtils::Material_set_color(mat, { col.x, col.y, col.z, col.w });
            UnityUtils::Material_set_renderQueue(mat, 5000);
            });
    }

    void ScanMaterials()
    {
        if (!self) return;
        self->materials_.clear();
        self->scanFilled_ = false;

        constexpr ImVec4 kDefaultColor = { 0.48f, 0.48f, 0.86f, 1.f };

        ForEachMaterial([](void* mat) {
            const auto name = GetMaterialName(mat);
            if (name.empty()) return;

            const bool exists = std::any_of(self->materials_.begin(), self->materials_.end(),
                [&](const CapsuleEntry& e) { return e.label == name; });

            if (!exists)
                self->materials_.push_back({ name, kDefaultColor });
            });

        self->scanFilled_ = !self->materials_.empty();
    }

    UPDATE_HOOK(Chams)
    {
        ApplyChams();
    }

}

float ChamsFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    const ImVec2 buttonSize = { ImClamp(width - 20.f, 160.f, width), 32.f };
    ImGui::SetCursorScreenPos({ x, y });
    if (ImGui::Button(XS("Scan##chams_scan"), buttonSize))
        ScanMaterials();
    y += buttonSize.y + 10.f;

    if (scanFilled_ && !materials_.empty())
    {
        const float chipH = GuiCapsuleListRow("chams_capsules_all", materials_, ImVec2(x, y), width);
        if (chipH > 0.f)
            y += chipH + 12.f;
    }

    return (y - detailStart.y) + 12.f;
}

void ChamsFeature::OnShutdown()
{
    SetEnabled(false);
}