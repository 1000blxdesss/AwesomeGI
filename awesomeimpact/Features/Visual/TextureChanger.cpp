#include "TextureChanger.h"
#include "../../GUI/gui.h"
#include "../../GUI/imgui_internal.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../Utils/stb_image_write.h"

#include <cctype>
#include <string>
#include <vector>

static TextureChangerFeature* self{ nullptr };
TextureChangerFeature::TextureChangerFeature()
    : Feature({ "card_texture_changer", XS("Texture Changer"), XS("Override character textures"), "NONE", FeatureTab::Visual })
{
    self = this;
}

namespace {

    inline std::string CleanLabel(const std::string& s)
    {
        auto r = s;
        for (const char* suffix : { XS(" (Instance)"), XS(" (instance)") })
        {
            const auto pos = r.find(suffix);
            if (pos != std::string::npos) r.erase(pos);
        }
        const auto h = r.rfind(" #");
        if (h != std::string::npos && h + 2 < r.size())
        {
            bool digits = true;
            for (size_t i = h + 2; i < r.size() && digits; ++i)
                digits = std::isdigit(static_cast<unsigned char>(r[i]));
            if (digits) r.erase(h);
        }
        return r;
    }

    inline std::string ExtractMatToken(const std::string& matName)
    {
        const auto cleaned = CleanLabel(matName);
        const auto us = cleaned.find_last_of('_');
        return (us != std::string::npos && us + 1 < cleaned.size())
            ? cleaned.substr(us + 1) : cleaned;
    }

    inline std::string ToLower(const std::string& s)
    {
        auto out = s;
        for (auto& c : out) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return out;
    }

    inline std::string SanitizeFilename(const std::string& name)
    {
        auto r = name;
        for (auto& c : r)
            if (c == ':' || c == '/' || c == '\\' || c == '*' || c == '?' || c == '<' || c == '>' || c == '|')
                c = '_';
        return r;
    }

    inline std::string GetObjectNameUtf8(void* obj)
    {
        auto* str = UnityUtils::Object_get_name(obj);
        return (str && str->length > 0) ? OtherUtils::Utf16ToUtf8(str->chars, str->length) : std::string();
    }

    bool DumpSingleTexture(void* tex, const std::string& filename)
    {
        if (!tex) return false;
        const int w = UnityUtils::Texture_get_width(tex);
        const int h = UnityUtils::Texture_get_height(tex);
        if (w <= 0 || h <= 0) return false;

        auto* tempRT = UnityUtils::RenderTexture_GetTemporary(w, h, 0);
        if (!tempRT) return false;

        auto* prevRT = UnityUtils::RenderTexture_get_active();
        UnityUtils::Graphics_Blit(tex, tempRT);
        UnityUtils::RenderTexture_set_active(tempRT);

        auto* newTex = malloc(1024);
        if (!newTex) {
            UnityUtils::RenderTexture_set_active(prevRT);
            UnityUtils::RenderTexture_ReleaseTemporary(tempRT);
            return false;
        }
        std::memset(newTex, 0, 1024);
        *reinterpret_cast<void**>(newTex) = *reinterpret_cast<void**>(tex);
        UnityUtils::Texture2D_ctor(newTex, w, h, 4, false);

        Rect rect = { 0, 0, static_cast<float>(w), static_cast<float>(h) };
        UnityUtils::Texture2D_ReadPixels(newTex, &rect, 0, 0, false);
        UnityUtils::Texture2D_Apply(newTex);

        auto* pixels = UnityUtils::Texture2D_GetPixels32(newTex, 0);
        UnityUtils::RenderTexture_set_active(prevRT);
        UnityUtils::RenderTexture_ReleaseTemporary(tempRT);

        if (!pixels || pixels->max_length != static_cast<uint32_t>(w * h)) {
            UnityUtils::Object_Destroy(newTex);
            free(newTex);
            return false;
        }

        auto* src = reinterpret_cast<Color32*>(pixels->vector);
        std::vector<unsigned char> img(w * h * 4);
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x) {
                const int si = y * w + x;
                const int di = (h - 1 - y) * w + x;
                img[di * 4] = src[si].r;
                img[di * 4 + 1] = src[si].g;
                img[di * 4 + 2] = src[si].b;
                img[di * 4 + 3] = src[si].a;
            }

        UnityUtils::Object_Destroy(newTex);
        free(newTex);
        return stbi_write_png(filename.c_str(), w, h, 4, img.data(), w * 4) != 0;
    }

    void ScanTextureRenderers()
    {
        if (!self) return;

        struct Guard {
            ~Guard() { if (owner) owner->scanQueued_.store(false, std::memory_order_release); }
            TextureChangerFeature* owner;
        } guard{ self };

        std::vector<TextureTargetEntry> out;

        const auto localAvatar = OtherUtils::AvatarManager();
        if (!localAvatar) goto Finish;

        {
            auto* avatarGO = UnityUtils::BaseEntity_rootObject(localAvatar);
            if (!avatarGO) goto Finish;

            auto* toolName = UnityUtils::PtrToStringAnsi((void*)XS("MonoVisualEntityTool"));
            auto* visualTool = UnityUtils::GameObject_GetComponentByName(avatarGO, toolName);
            if (!visualTool) goto Finish;

            auto* renderers = UnityUtils::MonoVisualEntityTool_get_renderers(visualTool);
            if (!renderers || renderers->max_length == 0 || renderers->max_length > 1000) goto Finish;

            for (uint32_t i = 0; i < renderers->max_length; ++i)
            {
                auto* renderer = renderers->vector[i];
                if (!renderer) continue;

                auto* rendererGO = UnityUtils::Component_get_gameObject(renderer);
                if (!rendererGO) continue;

                auto rendererName = GetObjectNameUtf8(rendererGO);
                auto partLabel = rendererName;
                constexpr auto kPartPrefix = "Part";
                if (partLabel.rfind(kPartPrefix, 0) == 0 && partLabel.size() > 4)
                    partLabel = partLabel.substr(4);

                auto* mats = UnityUtils::Renderer_get_sharedMaterials(renderer);
                if (!mats || mats->max_length == 0 || mats->max_length > 256) continue;

                for (uint32_t m = 0; m < mats->max_length; ++m)
                {
                    auto* mat = mats->vector[m];
                    if (!mat) continue;

                    const auto matName = GetObjectNameUtf8(mat);
                    if (matName.empty()) continue;

                    const auto token = ExtractMatToken(matName);
                    auto matLabel = ToLower(token);
                    if (_stricmp(token.c_str(), XS("Face")) == 0)
                        matLabel = XS("Face");

                    TextureTargetEntry entry;
                    entry.rendererName = matName;
                    entry.displayName = partLabel + " (" + matLabel + ")";
                    entry.material = mat;
                    entry.renderer = renderer;
                    entry.matIndex = m;
                    entry.path.fill(0);
                    out.push_back(std::move(entry));
                }
            }
        }

    Finish:
        std::lock_guard lock(self->pendingMutex_);
        self->pendingTargets_ = std::move(out);
        self->pendingScanFilled_ = !self->pendingTargets_.empty();
        self->pendingSwap_.store(true, std::memory_order_release);
    }

    inline void ApplyTextureOverride(void* material, const char* path)
    {
        if (!material || !path || !path[0]) return;

        auto* tex = UnityUtils::Material_get_mainTexture(material);
        if (!tex) return;

        auto* bytes = UnityUtils::File_ReadAllBytes(UnityUtils::PtrToStringAnsi((void*)path));
        if (!bytes || bytes->max_length == 0) return;

        UnityUtils::ImageConversion_LoadImageData(tex, bytes);
    }

    void DumpMaterialTextures(void* material, const std::string& baseName)
    {
        if (!material) return;

        constexpr auto kDir = "TextureDump";
        CreateDirectoryA(kDir, nullptr);

        auto* propNames = UnityUtils::Material_GetTexturePropertyNames(material);
        if (!propNames || propNames->max_length == 0)
        {
            auto* mainTex = UnityUtils::Material_get_mainTexture(material);
            if (!mainTex) return;

            const auto fn = std::string(kDir) + "\\" + SanitizeFilename(baseName) + "_mainTex.png";
            if (self) self->statusText_ = DumpSingleTexture(mainTex, fn) ? XS("yay!") : XS("Failed");
            return;
        }

        int dumped = 0;
        std::string lastFile;

        for (uint32_t i = 0; i < propNames->max_length; ++i)
        {
            auto* propStr = reinterpret_cast<Il2CppString*>(propNames->vector[i]);
            if (!propStr) continue;

            auto* tex = UnityUtils::Material_GetTexture(material, propStr);
            if (!tex) continue;

            const auto propName = OtherUtils::Utf16ToUtf8(propStr->chars, propStr->length);
            auto fn = std::string(kDir) + "\\" + SanitizeFilename(baseName) + "_" + SanitizeFilename(propName) + ".png";

            if (DumpSingleTexture(tex, fn)) { ++dumped; lastFile = std::move(fn); }
        }

        if (self)
            self->statusText_ = dumped > 0
            ? std::string(XS("Written to: ")) + lastFile + XS(" (") + std::to_string(dumped) + XS(" files)")
            : std::string(XS("Failed"));
    }

}

float TextureChangerFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    if (pendingSwap_.exchange(false, std::memory_order_acq_rel))
    {
        std::lock_guard lock(pendingMutex_);
        targets_.swap(pendingTargets_);
        scanFilled_ = pendingScanFilled_;
    }

    const ImVec2 buttonSize = { ImClamp(width - 20.f, 160.f, width), 32.f };
    ImGui::SetCursorScreenPos({ x, y });
    if (ImGui::Button(XS("Scan##texture_scan"), buttonSize))
        if (!scanQueued_.exchange(true, std::memory_order_acq_rel))
            RunOnUnityThread([] { ScanTextureRenderers(); });
    y += buttonSize.y + 10.f;

    if (!scanFilled_ || targets_.empty())
    {
        y += ImGui::GetFontSize() + 10.f;
    }
    else
    {
        for (size_t i = 0; i < targets_.size(); ++i)
        {
            auto& entry = targets_[i];
            bool apply = false, dump = false;

            y += GuiTextureInputRow(
                (std::string("tex_slot_") + std::to_string(i)).c_str(),
                entry.displayName.c_str(), entry,
                apply, dump, ImVec2(x, y), width) + 6.f;

            if (dump)
            {
                void* mat = entry.material;
                auto name = entry.displayName;
                RunOnUnityThread([mat, name] { DumpMaterialTextures(mat, name); });
            }

            if (apply)
            {
                void* mat = entry.material;
                std::string path = entry.path.data();
                if (!path.empty())
                    RunOnUnityThread([mat, path] { ApplyTextureOverride(mat, path.c_str()); });
            }
        }
    }

    if (!statusText_.empty())
    {
        ImGui::SetCursorScreenPos({ x, y });
        const bool failed = statusText_ == XS("Failed");
        ImGui::PushStyleColor(ImGuiCol_Text, failed
            ? ImVec4(1, 0.4f, 0.4f, 0.9f)
            : ImVec4(0.6f, 1, 0.6f, 0.75f));
        ImGui::TextUnformatted(statusText_.c_str());
        ImGui::PopStyleColor();
        y += ImGui::GetFontSize() + 8.f;
    }

    return (y - detailStart.y) + 12.f;
}

void TextureChangerFeature::OnShutdown()
{
    SetEnabled(false);
}
