#include "Minecraft.h"
#include "../../GUI/gui.h"
#include "../../core/App.h"
#include "../../globals.h"
#include "../../Utils/Embedded/minecraft_block_blob.h"
#include "../../Utils/test02.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>

extern bool g_showSettings;

namespace {

    MinecraftFeature* self = nullptr;

    void* g_previewCube = nullptr;
    void* g_previewTransform = nullptr;
    float  g_previewDistance = 4.f;
    int    g_groundMask = 0;
    bool   g_prevPlaceDown = false;
    void* g_blockTexture = nullptr;
    bool   g_blockPngWritten = false;
    std::string g_blockPngPath;

    constexpr int   kBuilderLayer = 11; // 9 - 15 // 
    constexpr float kMinDistance = 1.f;
    constexpr float kMaxDistance = 30.f;
    constexpr float kDistanceStep = 0.5f;


    inline Il2CppString* CachedStr(const char* s)
    {
        static std::unordered_map<const char*, Il2CppString*> cache;
        auto& entry = cache[s];
        if (!entry) entry = UnityUtils::PtrToStringAnsi((void*)s);
        return entry;
    }

    inline Vector3 NormalizeOr(Vector3 v, Vector3 fallback)
    {
        const float mag = v.magnitude();
        return mag > 0.0001f ? v * (1.f / mag) : fallback;
    }

    inline int ResolveGroundMask()
    {
        int base = UnityUtils::Miscs_GetSceneGroundLayerMaskWithoutTemp();
        if (base == 0) base = UnityUtils::Miscs_GetSceneGroundLayerMask();
        const int extra = UnityUtils::Miscs_GetExtraSurfaceLayerMask();
        return extra ? (base | extra) : base;
    }

    inline float SampleGroundY(float x, float z, float hintY)
    {
        if (!g_groundMask) g_groundMask = ResolveGroundMask();

        if (g_groundMask)
        {
            const float y = UnityUtils::Miscs_CalcCurrentGroundHeight(x, z, hintY + 120.f, 400.f, g_groundMask);
            if (std::isfinite(y)) return y;
        }

        const float y = UnityUtils::CalcCurrentGroundHeight(x, z);
        return std::isfinite(y) ? y : hintY;
    }

    inline void* FindRenderer(void* go)
    {
        if (!go) return nullptr;
        auto* r = UnityUtils::GameObject_GetComponentByName(go, CachedStr(XS("MeshRenderer")));
        if (!r) r = UnityUtils::GameObject_GetComponentByName(go, CachedStr(XS("SkinnedMeshRenderer")));
        if (!r) r = UnityUtils::GameObject_GetComponentByName(go, CachedStr(XS("Renderer")));
        return r;
    }

    void ApplyPreviewMaterial(void* go)
    {
        auto* renderer = FindRenderer(go);
        if (!renderer) return;

        auto* material = UnityUtils::Renderer_get_material(renderer);
        if (!material) return;

        const char* shaderNames[] = {
           // XS("Unlit/Transparent"),
            XS("Legacy Shaders/Transparent/Diffuse"),
            XS("Legacy Shaders/Transparent/VertexLit"),
            XS("Unlit/Color"),
        };
        void* shader = nullptr;
        for (const auto* name : shaderNames)
        {
            shader = UnityUtils::Shader_Find(CachedStr(name));
            if (shader) break;
        }
        if (shader) UnityUtils::Material_set_shader(material, shader);

        
        UnityUtils::Material_set_color(material, { 0.2f, 1.f, 0.2f, 0.22f });
        UnityUtils::Material_set_renderQueue(material, 3000);

        static int pidZWrite = UnityUtils::Shader_PropertyToID(CachedStr(XS("_ZWrite")));
        static int pidSrcBlend = UnityUtils::Shader_PropertyToID(CachedStr(XS("_SrcBlend")));
        static int pidDstBlend = UnityUtils::Shader_PropertyToID(CachedStr(XS("_DstBlend")));
        static int pidMode = UnityUtils::Shader_PropertyToID(CachedStr(XS("_Mode")));
        static int pidSurface = UnityUtils::Shader_PropertyToID(CachedStr(XS("_Surface")));
        static int pidAlphaClip = UnityUtils::Shader_PropertyToID(CachedStr(XS("_AlphaClip")));

        struct { int pid; int val; } props[] = {
            { pidZWrite,   0 }, { pidSrcBlend, 5 }, { pidDstBlend, 10 },
            { pidMode,     3 }, { pidSurface,  1 }, { pidAlphaClip, 0 },
        };
        for (const auto& p : props)
            if (p.pid) UnityUtils::Material_SetInt(material, p.pid, p.val);
    }

    bool EnsurePreviewCube()
    {
        if (g_previewCube && g_previewTransform) return true;

        g_previewCube = UnityUtils::GameObject_CreatePrimitive(PrimitiveType::Cube);
        if (!g_previewCube) return false;

        g_previewTransform = UnityUtils::GameObject_GetComponentByName(g_previewCube, CachedStr(XS("Transform")));
        if (!g_previewTransform)
        {
            UnityUtils::Object_Destroy(g_previewCube);
            g_previewCube = nullptr;
            return false;
        }

        const float sz = self ? self->GetBlockSize() : 1.f;
        UnityUtils::Transform_set_localScale(g_previewTransform, { sz, sz, sz });
        UnityUtils::GameObject_set_layer(g_previewCube, kBuilderLayer);

        auto* collider = UnityUtils::GameObject_GetComponentByName(g_previewCube, CachedStr(XS("Collider"))); // уже на усмотрение
        if (collider) UnityUtils::Collider_set_enabled(collider, false);

        ApplyPreviewMaterial(g_previewCube);
        UnityUtils::GameObject_SetActive(g_previewCube, true);
        return true;
    }

    inline void DestroyPreviewCube()
    {
        if (g_previewCube) UnityUtils::Object_Destroy(g_previewCube);
        g_previewCube = nullptr;
        g_previewTransform = nullptr;
    }

    void UpdatePreviewPose()
    {
        if (!EnsurePreviewCube()) return;

        auto* camera = UnityUtils::GetMainCamera();
        if (!camera) return;

        auto* camTr = UnityUtils::Component_get_transform(camera);
        if (!camTr) return;

        const auto camPos = UnityUtils::Transform_get_position(camTr);
        const auto camFwd = NormalizeOr(UnityUtils::Transform_get_forward(camTr), { 0, 0, 1 });

        const float sz = self ? self->GetBlockSize() : 1.f;

        auto target = camPos + camFwd * g_previewDistance;
        const float groundY = SampleGroundY(target.x, target.z, target.y);
        target.y = (std::max)(target.y, groundY + sz * 0.5f);

        target.x = std::round(target.x / sz) * sz;
        target.y = std::round(target.y / sz) * sz;
        target.z = std::round(target.z / sz) * sz;

        UnityUtils::Transform_set_localScale(g_previewTransform, { sz, sz, sz });
        UnityUtils::Transform_set_position(g_previewTransform, target);
    }

    bool EnsureBlockTexture()
    {
        if (g_blockTexture) return true;

        if (!g_blockPngWritten)
        {
            const char* customPath = self ? self->GetTexturePath() : nullptr;
            if (customPath && customPath[0])
            {
                auto* probe = UnityUtils::File_ReadAllBytes(UnityUtils::PtrToStringAnsi((void*)customPath));
                if (probe && probe->max_length > 0)
                {
                    g_blockPngPath = customPath;
                    g_blockPngWritten = true;
                }
            }

            if (!g_blockPngWritten)
            {
                char tempPath[MAX_PATH]{};
                if (!GetTempPathA(MAX_PATH, tempPath)) return false;

                g_blockPngPath = std::string(tempPath) + XS("awesomeimpact_minecraft_block.png");
                std::ofstream out(g_blockPngPath, std::ios::binary | std::ios::trunc);
                if (!out.good()) return false;
                out.write(
                    reinterpret_cast<const char*>(embedded_minecraft_block::data),
                    static_cast<std::streamsize>(embedded_minecraft_block::size));
                out.close();
                g_blockPngWritten = true;
            }
        }

        auto* baseTex = UnityUtils::Texture2D_get_blackTexture();
        if (!baseTex) return false;

        auto* tex = calloc(1, 0x800);
        if (!tex) return false;

        *reinterpret_cast<void**>(tex) = *reinterpret_cast<void**>(baseTex);
        UnityUtils::Texture2D_ctor(tex, 2, 2, 4, false);

        auto* bytes = UnityUtils::File_ReadAllBytes(UnityUtils::PtrToStringAnsi((void*)g_blockPngPath.c_str()));
        if (!bytes || bytes->max_length == 0) { free(tex); return false; }
        if (!UnityUtils::ImageConversion_LoadImage(tex, bytes)) { free(tex); return false; }

        g_blockTexture = tex;
        return true;
    }

    inline void InvalidateBlockTexture()
    {
        if (g_blockTexture) { free(g_blockTexture); g_blockTexture = nullptr; }
        g_blockPngWritten = false;
        g_blockPngPath.clear();
    }

    void PlaceBlockAtPreview()
    {
        if (!g_previewTransform || !EnsureBlockTexture()) return;

        const auto pos = UnityUtils::Transform_get_position(g_previewTransform);
        const float sz = self ? self->GetBlockSize() : 1.f;

        auto* go = UnityUtils::GameObject_CreatePrimitive(PrimitiveType::Cube);
        if (!go) return;

        auto* tr = UnityUtils::GameObject_GetComponentByName(go, CachedStr(XS("Transform")));
        if (!tr) return;

        UnityUtils::Transform_set_localScale(tr, { sz, sz, sz });
        UnityUtils::Transform_set_position(tr, pos);
        UnityUtils::GameObject_set_layer(go, kBuilderLayer);

        auto* collider = UnityUtils::GameObject_GetComponentByName(go, CachedStr(XS("Collider")));
        if (collider) UnityUtils::Collider_set_enabled(collider, true);

        auto* renderer = FindRenderer(go);
        if (!renderer) return;

        auto* material = UnityUtils::Renderer_get_material(renderer);
        if (!material) return;

        auto* texShader = UnityUtils::Shader_Find(CachedStr(XS("Unlit/Texture")));
        if (texShader) UnityUtils::Material_set_shader(material, texShader);

        UnityUtils::Material_SetTexture(material, CachedStr(XS("_MainTex")), g_blockTexture);
        UnityUtils::Material_SetTexture(material, CachedStr(XS("_BaseMap")), g_blockTexture);
    }

}

MinecraftFeature::MinecraftFeature()
    : Feature({ "card_minecraft_builder", XS("Minecraft Builder"), XS(":p"), "NONE", FeatureTab::Misc })
{
    self = this;
}

void MinecraftFeature::OnEnable()
{
    g_groundMask = 0;
    g_previewDistance = 4.f;
    g_prevPlaceDown = false;
}

void MinecraftFeature::OnUpdate()
{
    const int wheel = ConsumeGlobalMouseWheelSteps();
    if (wheel)
        g_previewDistance = std::clamp(
            g_previewDistance + static_cast<float>(wheel) * kDistanceStep,
            kMinDistance, kMaxDistance);

    UpdatePreviewPose();

    const bool placeDown =
        (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 ||
        (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;

    if (placeDown && !g_prevPlaceDown && !g_showSettings)
        PlaceBlockAtPreview();
    g_prevPlaceDown = placeDown;
}

float MinecraftFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiSliderRow("minecraft_block_size", XS("Block Size"),
        &blockSize_, 0.25f, 4.f, "%.2f", ImVec2(x, y), width, true) + 6.f;

    bool texConfirmed = false;
    y += GuiInputConfirmRow("minecraft_tex_path", XS("Texture"),
        texturePath_.data(), texturePath_.size(), texConfirmed, ImVec2(x, y), width) + 6.f;

    if (texConfirmed)
        InvalidateBlockTexture();

    ImGui::SetCursorScreenPos({ x, y });
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.65f, 1, 0.65f, 0.85f));
    ImGui::TextUnformatted(XS("Mouse wheel: move preview cube closer/farther"));
    ImGui::PopStyleColor();
    y += ImGui::GetFontSize() + 6.f;

    ImGui::SetCursorScreenPos({ x, y });
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.65f, 1, 0.65f, 0.85f));
    ImGui::TextUnformatted(XS("Place block: RMB or MMB"));
    ImGui::PopStyleColor();
    y += ImGui::GetFontSize() + 6.f;

   
   /* ImGui::SetCursorScreenPos({ x, y });
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 0.62f));
    ImGui::TextUnformatted(buf);
    ImGui::PopStyleColor();
    y += ImGui::GetFontSize() + 10.f;*/

    return (y - detailStart.y) + 12.f;
}

void MinecraftFeature::OnShutdown()
{
    DestroyPreviewCube();
    InvalidateBlockTexture();
    g_prevPlaceDown = false;
    SetEnabled(false);
}