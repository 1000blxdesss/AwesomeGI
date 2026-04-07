#include "PenisDraw.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#include <cstring>

static PenisDrawFeature* self{ nullptr };
PenisDrawFeature::PenisDrawFeature()
    : Feature({ "penis_draw", XS("Penis Draw"), XS("I know what you're going to do with it"), "NONE", FeatureTab::Misc })
{
    self = this;
}

namespace {

    void* g_penisGO = nullptr;
    void* g_penisTransform = nullptr;
    void* g_penisTex = nullptr;

    inline void DestroyPenis()
    {
        if (g_penisGO)
        {
            UnityUtils::Object_Destroy(g_penisGO);
            g_penisGO = nullptr;
            g_penisTransform = nullptr;
        }
        if (g_penisTex)
        {
            free(g_penisTex);
            g_penisTex = nullptr;
        }
    }

    inline void* GetRenderer()
    {
        if (!g_penisGO) return nullptr;
        auto* rName = UnityUtils::PtrToStringAnsi((void*)XS("Renderer"));
        return UnityUtils::GameObject_GetComponentByName(g_penisGO, rName);
    }

    void ApplyTexture()
    {
        auto* renderer = GetRenderer();
        if (!renderer) return;

        auto* mat = UnityUtils::Renderer_get_material(renderer);
        if (!mat) return;

        auto* bytes = UnityUtils::File_ReadAllBytes(UnityUtils::PtrToStringAnsi((void*)self->texturePath_.data()));
        if (!bytes || bytes->max_length == 0) return;

        auto* baseTex = UnityUtils::Texture2D_get_blackTexture();
        if (!baseTex) return;

        if (g_penisTex) { free(g_penisTex); g_penisTex = nullptr; }

        // или можем посмотреть в сторону sub_141608E70
        auto* tex = malloc(0x800);
        std::memset(tex, 0, 0x800);
        *reinterpret_cast<void**>(tex) = *reinterpret_cast<void**>(baseTex);
        UnityUtils::Texture2D_ctor(tex, 2, 2, 4, false);

        if (UnityUtils::ImageConversion_LoadImage(tex, bytes))
        {
            auto* mainTex = UnityUtils::PtrToStringAnsi((void*)XS("_MainTex"));
            UnityUtils::Material_SetTexture(mat, mainTex, tex);
            g_penisTex = tex;
            self->textureApplied_ = false;
        }
        else
        {
            free(tex);
        }
    }

    UPDATE_HOOK(Penis)
    {
        if (!self || !self->Active())
        {
            if (g_penisGO) UnityUtils::GameObject_SetActive(g_penisGO, false);
            return;
        }

        auto* avatar = OtherUtils::AvatarManager();
        if (!avatar) return;

        auto* avatarObj = UnityUtils::BaseEntity_rootObject(avatar);
        if (!avatarObj) return;

        auto* trName = UnityUtils::PtrToStringAnsi((void*)XS("Transform"));
        auto* avatarTr = UnityUtils::GameObject_GetComponentByName(avatarObj, trName);
        if (!avatarTr) return;

        auto* hipsPath = UnityUtils::PtrToStringAnsi((void*)XS("Bip001/Bip001 Pelvis"));
        auto* hipsFall = UnityUtils::PtrToStringAnsi((void*)XS("Bip001 Pelvis"));

        auto* hipsBone = UnityUtils::Transform_Find(avatarTr, hipsPath);
        if (!hipsBone) hipsBone = UnityUtils::Transform_Find(avatarTr, hipsFall);
        if (!hipsBone) return;

        if (!g_penisGO)
        {
            g_penisGO = UnityUtils::GameObject_CreatePrimitive(PrimitiveType::Capsule);
            if (!g_penisGO) return;

            g_penisTransform = UnityUtils::GameObject_GetComponentByName(g_penisGO, trName);
            if (!g_penisTransform) return;

            UnityUtils::Transform_SetParent(g_penisTransform, hipsBone, false);
        }

        if (self->textureApplied_ && self->texturePath_[0] != '\0') ApplyTexture();
        if (!g_penisTransform) return;

        UnityUtils::GameObject_SetActive(g_penisGO, true);

        const float size = self->GetSize();
        const float thick = self->GetThickness();

        UnityUtils::Transform_set_localPosition(g_penisTransform, { 0.f, size * 0.7f, 0.f });
        UnityUtils::Transform_set_localScale(g_penisTransform, { thick, size, thick });
    }

}

float PenisDrawFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiSliderRow("penis_size", XS("Size"),
        &size_, 0.05f, 0.27f, "%.2f", ImVec2(x, y), width, true) + 6.f;

    y += GuiSliderRow("penis_thickness", XS("Thickness"),
        &thickness_, 0.01f, 0.3f, "%.2f", ImVec2(x, y), width, true) + 6.f;

    y += GuiInputConfirmRow("penis_tex_path", XS("Texture"),
        texturePath_.data(), texturePath_.size(),
        textureApplied_, ImVec2(x, y), width) + 8.f;

    y += GuiToggleTextRow("penis_hold", XS("Key"),
        XS("Hold"), hold_, ImVec2(x, y), width) + 8.f;

    return (y - detailStart.y) + 12.f;
}

void PenisDrawFeature::OnShutdown()
{
    DestroyPenis();
    SetEnabled(false);
}
