#include "CameraSettings.h"
#include "../../GUI/gui.h"
#include "../../Offsets.h"
#include "../../globals.h"
#include "../Misc/KeepCamera.h"
#include "../../Utils/test02.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

static CameraSettingsFeature* self{ nullptr };

CameraSettingsFeature::CameraSettingsFeature()
    : Feature({ "card_camera", XS("Camera Setting"), XS("Adjust camera parameters"), "NONE", FeatureTab::Visual })
{
    self = this;
}

namespace {

    bool g_faceHidden = false;
    std::vector<void*> g_faceRenderers;

    inline std::string GetObjectName(void* obj)
    {
        auto* str = UnityUtils::Object_get_name(obj);
        if (!str) return {};
        return OtherUtils::Utf16ToUtf8(str->chars, str->length);
    }

    inline bool ContainsCI(std::string text, const char* token)
    {
        if (!token || !*token) return false;
        std::transform(text.begin(), text.end(), text.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        std::string key(token);
        std::transform(key.begin(), key.end(), key.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return text.find(key) != std::string::npos;
    }

    inline bool IsHeadPart(const std::string& name)
    {
        return ContainsCI(name, XS("_face"))
            || ContainsCI(name, XS("face"))
            || ContainsCI(name, XS("head"))
            || ContainsCI(name, XS("hair"))
            || ContainsCI(name, XS("eye"));
    }

    inline bool IsFaceRenderer(void* renderer)
    {
        if (!renderer) return false;

        auto* mats = UnityUtils::Renderer_get_sharedMaterials(renderer);
        if (!mats || mats->max_length == 0 || mats->max_length > 256)
            return false;

        bool hasHead = false;
        bool hasNonHead = false;

        for (uint32_t i = 0; i < mats->max_length; ++i)
        {
            auto* mat = mats->vector[i];
            if (!mat) continue;

            if (IsHeadPart(GetObjectName(mat)))
                hasHead = true;
            else
                hasNonHead = true;
        }

        if (hasHead && !hasNonHead)
            return true;

        if (mats->max_length == 1)
        {
            auto* go = UnityUtils::Component_get_gameObject(renderer);
            if (go) return IsHeadPart(GetObjectName(go));
        }

        return false;
    }

    void RestoreFaceRenderers()
    {
        if (!g_faceHidden) return;

        for (auto* renderer : g_faceRenderers)
            if (renderer) UnityUtils::Renderer_set_enabled(renderer, true);

        g_faceRenderers.clear();
        g_faceHidden = false;
    }

    void HideFaceRenderers(void* localAvatar)
    {
        if (!localAvatar || g_faceHidden) return;

        auto* avatarGO = UnityUtils::BaseEntity_rootObject(localAvatar);
        if (!avatarGO) return;

        static Il2CppString* toolName =
            UnityUtils::PtrToStringAnsi((void*)XS("MonoVisualEntityTool"));

        auto* visualTool = UnityUtils::GameObject_GetComponentByName(avatarGO, toolName);
        if (!visualTool) return;

        auto* renderers = UnityUtils::MonoVisualEntityTool_get_renderers(visualTool);
        if (!renderers || renderers->max_length == 0 || renderers->max_length > 1000)
            return;

        g_faceRenderers.clear();
        for (uint32_t i = 0; i < renderers->max_length; ++i)
        {
            auto* renderer = renderers->vector[i];
            if (!renderer || !IsFaceRenderer(renderer)) continue;

            if (std::find(g_faceRenderers.begin(), g_faceRenderers.end(), renderer) != g_faceRenderers.end())
                continue;

            UnityUtils::Renderer_set_enabled(renderer, false);
            g_faceRenderers.push_back(renderer);
        }

        g_faceHidden = !g_faceRenderers.empty();
    }

    inline void ApplyFirstPerson(void* camera, void* localAvatar)
    {
        static Il2CppString* animatorName =
            UnityUtils::PtrToStringAnsi((void*)XS("Animator"));

        auto* animator = UnityUtils::GameObject_GetComponentByName(
            UnityUtils::BaseEntity_rootObject(localAvatar), animatorName);
        if (!animator) return;

        auto* headBone = UnityUtils::Animator_GetBoneTransform(
            animator, static_cast<int>(HumanBodyBones::Head));
        if (!headBone) return;

        auto* camTransform = UnityUtils::Component_get_transform(camera);
        if (!camTransform) return;

        HideFaceRenderers(localAvatar);

        auto headPos = UnityUtils::Transform_get_position(headBone);
        const auto camFwd = UnityUtils::Transform_get_forward(camTransform);

        constexpr float kHeadOffset = 0.2f;
        constexpr float kForwardBias = 0.2f;

        headPos.y += kHeadOffset;
        headPos.x += camFwd.x * kForwardBias;
        headPos.z += camFwd.z * kForwardBias;

        UnityUtils::Transform_set_position(camTransform, headPos);
    }

}

void (*FlushToOutpuCamera_Orig)(void*) = nullptr;
void FlushToOutpuCamera_Hook(void* __this)
{
    auto* brain = reinterpret_cast<CinemachineBrain*>(__this);
    KeepCamera_OnFlushPre(brain->m_OutputCamera);

    FlushToOutpuCamera_Orig(__this);

    auto* cam = brain->m_OutputCamera;

    if (self && self->Active() && cam)
    {
        UnityUtils::Camera_set_fieldOfView(cam, self->GetFov());
        UnityUtils::Camera_set_farClipPlane(cam, self->GetDrawDistance());

        bool fpApplied = false;
        if (self->FirstPersonEnabled())
        {
            if (const auto localAvatar = OtherUtils::AvatarManager())
            {
                ApplyFirstPerson(cam, localAvatar);
                fpApplied = true;
            }
        }

        if (!fpApplied)RestoreFaceRenderers();
    }
    else
    {
        RestoreFaceRenderers();
    }

    KeepCamera_OnFlushPost(cam);
}

float CameraSettingsFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiSliderRow("camera_fov", XS("FOV"),
        &fov_, -20.f, 140.f, "%.0f", ImVec2(x, y), width, true) + 10.f;

    y += GuiSliderRow("camera_draw", XS("Draw Dist"),
        &drawDistance_, -50.f, 2000.f, "%.0f", ImVec2(x, y), width, true) + 10.f;

    y += GuiToggleTextRow("camera_first_person_enabled", XS("FPV"),
        XS("Enabled"), firstPersonEnabled_, ImVec2(x, y), width) + 6.f;

    y += GuiToggleTextRow("camera_first_person_hold", XS("Key"),
        XS("Hold"), firstPersonHold_, ImVec2(x, y), width) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void CameraSettingsFeature::OnShutdown()
{
    RestoreFaceRenderers();
    SetEnabled(false);
}