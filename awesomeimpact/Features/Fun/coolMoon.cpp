#include "coolMoon.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/Embedded/awes_gif_blob.h"
#include "../../Utils/test02.h"

#include <cstring>
#include <vector>

static CoolMoonFeature* self{ nullptr };
CoolMoonFeature::CoolMoonFeature()
    : Feature({ "card_cool_moon", XS("Cool Moon"), XS("glaza pol...yt"), "NONE", FeatureTab::Misc })
{
    self = this;
}

namespace {

    struct GifFrame
    {
        std::vector<uint8_t> rgba;
        int delayMs = 100;
    };

    bool                  g_activated = false;
    bool                  g_disabled = false;
    std::vector<GifFrame> g_frames;
    size_t                g_frameIdx = 0;
    int                   g_gifW = 0, g_gifH = 0;
    void*                 g_moonTex = nullptr;
    void*                 g_cachedMoon = nullptr;
    void*                 g_cachedRenderer = nullptr;
    void*                 g_cachedMaterial = nullptr;
    void*                 g_originalShader = nullptr;
    void*                 g_originalMainTex = nullptr;
    void*                 g_originalBaseMap = nullptr;
    bool                  g_savedMaterialState = false;
    ULONGLONG             g_nextFrameAtMs = 0;

    inline void ForgetMoonBinding()
    {
        g_cachedMoon = nullptr;
        g_cachedRenderer = nullptr;
        g_cachedMaterial = nullptr;
        g_originalShader = nullptr;
        g_originalMainTex = nullptr;
        g_originalBaseMap = nullptr;
        g_savedMaterialState = false;
    }

    inline bool CanRestoreMoonMaterial()
    {
        if (!g_cachedMoon || !g_cachedMaterial)
            return false;

        auto* env = UnityUtils::EnviroSky_get_Instance();
        return env && env->Components && env->Components->Moon == g_cachedMoon;
    }

    inline void RestoreMoonMaterial()
    {
        if (g_savedMaterialState && CanRestoreMoonMaterial())
        {
            auto* mainTex = UnityUtils::PtrToStringAnsi((void*)XS("_MainTex"));
            auto* baseMap = UnityUtils::PtrToStringAnsi((void*)XS("_BaseMap"));

            if (g_originalShader)
                UnityUtils::Material_set_shader(g_cachedMaterial, g_originalShader);

            UnityUtils::Material_SetTexture(g_cachedMaterial, mainTex, g_originalMainTex);
            UnityUtils::Material_SetTexture(
                g_cachedMaterial,
                baseMap,
                g_originalBaseMap ? g_originalBaseMap : g_originalMainTex);
        }

        ForgetMoonBinding();
    }

    inline void DestroyMoonTexture()
    {
        if (!g_moonTex)
            return;

        UnityUtils::Object_Destroy(g_moonTex);
        free(g_moonTex);
        g_moonTex = nullptr;
    }

    inline void ResetGifState()
    {
        RestoreMoonMaterial();
        DestroyMoonTexture();
        g_frames.clear();
        g_frames.shrink_to_fit();
        g_frameIdx = 0;
        g_gifW = g_gifH = 0;
        g_disabled = false;
        g_activated = false;
        g_nextFrameAtMs = 0;
    }

    void* FindRendererRecursive(void* go)
    {
        if (!go) return nullptr;

        auto* meshName = UnityUtils::PtrToStringAnsi((void*)XS("MeshRenderer"));
        auto* skinnedName = UnityUtils::PtrToStringAnsi((void*)XS("SkinnedMeshRenderer"));
        auto* rendName = UnityUtils::PtrToStringAnsi((void*)XS("Renderer"));

        auto* r = UnityUtils::GameObject_GetComponentByName(go, meshName);
        if (!r) r = UnityUtils::GameObject_GetComponentByName(go, skinnedName);
        if (!r) r = UnityUtils::GameObject_GetComponentByName(go, rendName);
        if (r) return r;

        auto* tr = UnityUtils::Component_get_transform(go);
        if (!tr) return nullptr;

        const int count = UnityUtils::Transform_get_childCount(tr);
        for (int i = 0; i < count; ++i)
        {
            auto* childGo = UnityUtils::Component_get_gameObject(UnityUtils::Transform_GetChild(tr, i));
            r = FindRendererRecursive(childGo);
            if (r) return r;
        }
        return nullptr;
    }

    bool LoadGifFrames()
    {
        int* delays = nullptr;
        int width = 0, height = 0, frameCount = 0, comp = 0;

        auto* gifRgba = stbi_load_gif_from_memory(
            embedded_awes_gif::data, static_cast<int>(embedded_awes_gif::size),
            &delays, &width, &height, &frameCount, &comp, 4);

        if (!gifRgba || frameCount <= 0 || width <= 0 || height <= 0)
        {
            if (gifRgba) stbi_image_free(gifRgba);
            if (delays)  stbi_image_free(delays);
            return false;
        }

        g_gifW = width;
        g_gifH = height;

        const size_t frameBytes = static_cast<size_t>(width) * height * 4;
        const int rowBytes = width * 4;
        g_frames.reserve(frameCount);

        for (int i = 0; i < frameCount; ++i)
        {
            const auto* src = gifRgba + static_cast<size_t>(i) * frameBytes;
            GifFrame fr;
            fr.rgba.resize(frameBytes);
            fr.delayMs = delays ? delays[i] : 100;
            if (fr.delayMs <= 0)
                fr.delayMs = 100;

            for (int y = 0; y < height; ++y)
                std::memcpy(
                    fr.rgba.data() + static_cast<size_t>(y) * rowBytes,
                    src + static_cast<size_t>(height - 1 - y) * rowBytes,
                    rowBytes);

            g_frames.push_back(std::move(fr));
        }

        stbi_image_free(gifRgba);
        if (delays) stbi_image_free(delays);

        g_frameIdx = 0;
        return !g_frames.empty();
    }

    bool EnsureMoonTexture()
    {
        if (g_moonTex)
            return true;

        if (g_gifW <= 0 || g_gifH <= 0)
            return false;

        auto* baseTex = UnityUtils::Texture2D_get_blackTexture();
        if (!baseTex)
            return false;

        auto* tex = calloc(1, 0x800);
        if (!tex)
            return false;

        *reinterpret_cast<void**>(tex) = *reinterpret_cast<void**>(baseTex);
        UnityUtils::Texture2D_ctor(tex, g_gifW, g_gifH, 4, false);
        g_moonTex = tex;
        return true;
    }

    bool EnsureMoonMaterial()
    {
        auto* env = UnityUtils::EnviroSky_get_Instance();
        if (!env || !env->Components || !env->Components->Moon)
            return false;

        auto* moon = env->Components->Moon;
        if (moon == g_cachedMoon && g_cachedRenderer && g_cachedMaterial)
            return true;

        auto* renderer = FindRendererRecursive(moon);
        if (!renderer)
            return false;

        auto* mat = UnityUtils::Renderer_get_material(renderer);
        if (!mat)
            return false;

        ForgetMoonBinding();

        g_cachedMoon = moon;
        g_cachedRenderer = renderer;
        g_cachedMaterial = mat;

        auto* shaderName = UnityUtils::PtrToStringAnsi((void*)XS("Unlit/Texture"));
        auto* mainTex = UnityUtils::PtrToStringAnsi((void*)XS("_MainTex"));
        auto* baseMap = UnityUtils::PtrToStringAnsi((void*)XS("_BaseMap"));

        g_originalShader = UnityUtils::Material_get_shader(mat);
        g_originalMainTex = UnityUtils::Material_GetTexture(mat, mainTex);
        g_originalBaseMap = UnityUtils::Material_GetTexture(mat, baseMap);
        g_savedMaterialState = true;

        auto* shader = UnityUtils::Shader_Find(shaderName);
        if (shader)
            UnityUtils::Material_set_shader(mat, shader);

        UnityUtils::Material_SetTexture(mat, mainTex, g_moonTex);
        UnityUtils::Material_SetTexture(mat, baseMap, g_moonTex);
        return true;
    }

    void TickMoonGif()
    {
        if (!g_activated || g_disabled) return;

        if (g_frames.empty())
        {
            if (!LoadGifFrames()) { g_disabled = true; return; }
        }

        if (!EnsureMoonTexture()) { g_disabled = true; return; }
        if (!EnsureMoonMaterial()) return;

        const ULONGLONG now = GetTickCount64();
        if (g_nextFrameAtMs && now < g_nextFrameAtMs)
            return;

        if (g_frameIdx >= g_frames.size()) g_frameIdx = 0;
        auto& fr = g_frames[g_frameIdx];

        UnityUtils::Texture2D_LoadRawTextureData(g_moonTex, fr.rgba.data(), static_cast<int>(fr.rgba.size()));
        UnityUtils::Texture2D_Apply_2(g_moonTex, false, false);

        g_nextFrameAtMs = now + static_cast<ULONGLONG>(fr.delayMs);
        g_frameIdx = (g_frameIdx + 1) % g_frames.size();
    }

    UPDATE_HOOK(CoolMoon_Update)
    {
        if (!self || !self->Active()) return;
        TickMoonGif();
    }

}

float CoolMoonFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    ImGui::SetCursorScreenPos({ x, y });
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 0.58f));
    ImGui::TextUnformatted(XS(":3"));
    ImGui::PopStyleColor();
    y += ImGui::GetFontSize() + 12.f;

    return (y - detailStart.y) + 12.f;
}

void CoolMoonFeature::OnEnable()
{
    g_activated = true;
    g_disabled = false;
    g_nextFrameAtMs = 0;
}

void CoolMoonFeature::OnShutdown()
{
    ResetGifState();
    SetEnabled(false);
}
