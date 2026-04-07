#include "Atmosphere.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../GUI/imgui_internal.h"
#include "../../Utils/test02.h"

#include <array>
#include <cstdio>

static AtmosphereFeature* self{ nullptr };

namespace {

    inline void SaveConfig(const EnviroConfigFloat& src, EnviroConfigFloat& dst)
    {
        dst.valueType = src.valueType;
        dst.inputValue = src.inputValue;
    }

    inline void SaveConfig(const EnviroConfigColor& src, EnviroConfigColor& dst)
    {
        dst.colorType = src.colorType;
        dst.inputColor = src.inputColor;
    }

    inline void ApplyConfig(EnviroConfigFloat& target, float v)
    {
        target.valueType = EnviroConfigValueType::Value;
        target.inputValue = v;
    }

    inline void ApplyConfig(EnviroConfigColor& target, const ImVec4& v)
    {
        target.colorType = EnviroConfigColorType::Color;
        target.inputColor = { v.x, v.y, v.z, v.w };
    }

    inline void RestoreConfig(const EnviroConfigFloat& src, EnviroConfigFloat& dst)
    {
        dst.valueType = src.valueType;
        dst.inputValue = src.inputValue;
    }

    inline void RestoreConfig(const EnviroConfigColor& src, EnviroConfigColor& dst)
    {
        dst.colorType = src.colorType;
        dst.inputColor = src.inputColor;
    }

    inline ImVec4 ColorToVec4(const Color& c) { return { c.r, c.g, c.b, c.a }; }
    inline Color Vec4ToColor(const ImVec4& v) { return { v.x, v.y, v.z, v.w }; }

    struct LightExtraFloatDef { const char* label; uint32_t offset; };
    struct LightExtraColorDef { const char* label; uint32_t offset; };

    const LightExtraFloatDef kLightExtraFloatDefs[AtmosphereFeature::kLightExtraFloatCount] = {
        { XS("Post Threshold"),            static_cast<uint32_t>(offsetof(EnviroLightSettings, postThreshold))                },
        { XS("Post Threshold Character"),  static_cast<uint32_t>(offsetof(EnviroLightSettings, postThresholdCharacter))       },
        { XS("Post Exposure"),             static_cast<uint32_t>(offsetof(EnviroLightSettings, postProcessExposure))          },
        { XS("Far Light Intensity"),       static_cast<uint32_t>(offsetof(EnviroLightSettings, farLightIntensity))            },
        { XS("Post Scaler"),               static_cast<uint32_t>(offsetof(EnviroLightSettings, postScaler))                   },
        { XS("Vegetation Luminance"),      static_cast<uint32_t>(offsetof(EnviroLightSettings, vegetationTransitionLuminance))},
        { XS("Water Shallow Intensity"),   static_cast<uint32_t>(offsetof(EnviroLightSettings, waterShallowIntensity))        },
        { XS("Water Deep Intensity"),      static_cast<uint32_t>(offsetof(EnviroLightSettings, waterDeepIntensity))           },
        { XS("Water Reflection Intensity"),static_cast<uint32_t>(offsetof(EnviroLightSettings, waterReflectionIntensity))     },
        { XS("Grass Specular Scale"),      static_cast<uint32_t>(offsetof(EnviroLightSettings, grassSpecularScale))           },
        { XS("Grass Transmit Scale"),      static_cast<uint32_t>(offsetof(EnviroLightSettings, grassTransmitScale))           },
        { XS("Lyra Enable"),               static_cast<uint32_t>(offsetof(EnviroLightSettings, lyraGIEnable))                 },
    };

    const LightExtraColorDef kLightExtraColorDefs[AtmosphereFeature::kLightExtraColorCount] = {
        { XS("Water Shallow Color"), static_cast<uint32_t>(offsetof(EnviroLightSettings, waterShallowColor)) },
        { XS("Water Deep Color"),    static_cast<uint32_t>(offsetof(EnviroLightSettings, waterDeepColor))    },
    };


    inline void FormatColorHex(char* buf, size_t size, const ImVec4& c)
    {
        if (!buf || !size) return;
        _snprintf_s(buf, size, _TRUNCATE, "#%02X%02X%02X%02X",
            static_cast<int>(ImClamp(c.x, 0.f, 1.f) * 255.f + 0.5f),
            static_cast<int>(ImClamp(c.y, 0.f, 1.f) * 255.f + 0.5f),
            static_cast<int>(ImClamp(c.z, 0.f, 1.f) * 255.f + 0.5f),
            static_cast<int>(ImClamp(c.w, 0.f, 1.f) * 255.f + 0.5f));
    }

    inline bool ParseColorHex(const char* text, ImVec4& out)
    {
        if (!text) return false;

        auto nibble = [](char c) -> int {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
            if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
            return -1;
            };
        auto byte = [&](char hi, char lo) -> int {
            const int h = nibble(hi), l = nibble(lo);
            return (h < 0 || l < 0) ? -1 : (h << 4) | l;
            };

        const char* d = text[0] == '#' ? text + 1 : text;
        const int r = byte(d[0], d[1]), g = byte(d[2], d[3]);
        const int b = byte(d[4], d[5]), a = byte(d[6], d[7]);
        if (r < 0 || g < 0 || b < 0 || a < 0) return false;

        constexpr float inv = 1.f / 255.f;
        out = { r * inv, g * inv, b * inv, a * inv };
        return true;
    }

    float CloudColorPickerRow(const char* id, const char* label, ImVec4& pickerColor,
        ImVec4* targets, int targetCount, const char* const* targetLabels,
        const ImVec2& origin, float innerWidth)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const float fontSize = ImGui::GetFontSize();
        const float buttonSize = ImClamp(fontSize + 2.f, 16.f, 24.f);
        const float rowHeight = buttonSize + 8.f;

        float buttonX = ImMax(origin.x + innerWidth - buttonSize, origin.x + 90.f);
        const float buttonY = origin.y + (rowHeight - buttonSize) * 0.5f;

        ImGui::SetCursorScreenPos(origin);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.68f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImGui::PushID(id);
        const ImVec2 btnPos = { buttonX, buttonY };
        const ImVec2 btnSize = { buttonSize, buttonSize };

        ImGui::SetCursorScreenPos(btnPos);
        ImGui::InvisibleButton("##color_button", btnSize);
        const bool hovered = ImGui::IsItemHovered();
        const bool held = ImGui::IsItemActive();
        if (hovered) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        if (ImGui::IsItemClicked()) ImGui::OpenPopup("picker");

        const float rounding = ImClamp(buttonSize * 0.35f, 3.f, 8.f);
        const ImVec2 previewMin = btnPos;
        const ImVec2 previewMax = add(btnPos, btnSize);

#ifdef ImDrawFlags_RoundCornersAll
        ImGui::RenderColorRectWithAlphaCheckerboard(dl, previewMin, previewMax,
            ImGui::GetColorU32(pickerColor), 8.f, {}, rounding, ImDrawFlags_RoundCornersAll);
#else
        ImGui::RenderColorRectWithAlphaCheckerboard(dl, previewMin, previewMax,
            ImGui::GetColorU32(pickerColor), 8.f, {}, rounding);
#endif

        ImVec4 overlay = { 1.f, 1.f, 1.f, held ? 0.16f : hovered ? 0.08f : 0.f };
        if (overlay.w > 0.f)
            dl->AddRectFilled(previewMin, previewMax, ImGui::ColorConvertFloat4ToU32(overlay), rounding);

        const auto baseBorder = GuiColorOutline();
        auto highlight = GuiColorHighlight();
        highlight.w = ImClamp(highlight.w + 0.28f, 0.f, 1.f);
        const auto border = (hovered || held) ? ImLerp(baseBorder, highlight, 0.65f) : baseBorder;
        const ImVec2 inset = { 1.8f, 1.8f };
        dl->AddRect(sub(previewMin, inset), add(previewMax, inset),
            ImGui::ColorConvertFloat4ToU32(border),
            ImClamp(rounding + 1.65f, rounding + 1.2f, rounding + 2.f), 0, 1.8f);

        ImGui::SetNextWindowPos(
            { btnPos.x + btnSize.x * 0.5f, btnPos.y + btnSize.y + 8.f },
            ImGuiCond_Appearing, { 0.5f, 0.f });
        ImGui::SetNextWindowSize({ 600.f, 0.f }, ImGuiCond_Appearing);

        if (ImGui::BeginPopup("picker", ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
            ImGui::TextUnformatted(label);
            ImGui::Separator();

            auto* storage = ImGui::GetStateStorage();
            const auto selId = ImGui::GetID("cloud_picker_selected");
            int sel = storage->GetInt(selId, -1);

            static std::array<char, 16> hexBuf{};

            if (ImGui::IsWindowAppearing())
            {
                if (sel < 0 && targetCount > 0) sel = 0;
                if (sel >= 0 && sel < targetCount) pickerColor = targets[sel];
                storage->SetInt(selId, sel);
                FormatColorHex(hexBuf.data(), hexBuf.size(), pickerColor);
            }

            ImGui::BeginChild("cloud_picker_left", { 280.f, 320.f }, false);
            ImGui::Indent(8.f);
            ImGui::PushItemWidth(244.f);

            ImGui::TextUnformatted(XS("Hex Code"));
            ImGui::InputText("##color_hex", hexBuf.data(), hexBuf.size(), ImGuiInputTextFlags_CharsUppercase);
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                ImVec4 parsed;
                if (ParseColorHex(hexBuf.data(), parsed)) pickerColor = parsed;
                FormatColorHex(hexBuf.data(), hexBuf.size(), pickerColor);
                if (sel >= 0 && sel < targetCount) targets[sel] = pickerColor;
            }

            ImGui::Spacing();
            ImVec4 temp = pickerColor;
            if (ImGui::ColorPicker4("##color_picker", &temp.x,
                ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar |
                ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview |
                ImGuiColorEditFlags_AlphaPreviewHalf))
            {
                pickerColor = temp;
                FormatColorHex(hexBuf.data(), hexBuf.size(), pickerColor);
                if (sel >= 0 && sel < targetCount) targets[sel] = pickerColor;
            }

            ImGui::PopItemWidth();
            ImGui::Unindent(8.f);
            ImGui::EndChild();

            ImGui::SameLine();
            ImGui::BeginChild("cloud_picker_right", { 220.f, 320.f }, true);
            ImGui::Indent(6.f);
            ImGui::TextUnformatted(XS("Apply To"));
            ImGui::Separator();

            auto* listDl = ImGui::GetWindowDrawList();
            const float rowH = ImGui::GetFrameHeight() + 2.f;

            for (int i = 0; i < targetCount; ++i)
            {
                const float rowW = ImGui::GetContentRegionAvail().x;
                ImGui::PushID(i);
                const auto rowMin = ImGui::GetCursorScreenPos();
                const ImVec2 rowMax = { rowMin.x + rowW, rowMin.y + rowH };

                ImGui::InvisibleButton("##cloud_row", { rowW, rowH });

                if (ImGui::IsItemHovered())
                    listDl->AddRect(rowMin, rowMax, IM_COL32(255, 255, 255, 190), 4.f, 0, 1.4f);

                if (ImGui::IsItemClicked())
                {
                    sel = i;
                    storage->SetInt(selId, sel);
                    pickerColor = targets[sel];
                    FormatColorHex(hexBuf.data(), hexBuf.size(), pickerColor);
                }

                if (sel == i)
                    listDl->AddRectFilled(
                        { rowMin.x, rowMin.y + 2.f }, { rowMin.x + 3.f, rowMax.y - 2.f },
                        IM_COL32(255, 255, 255, 90), 2.f);

                ImVec4 textCol = targets[i]; textCol.w = 1.f;
                const auto textSz = ImGui::CalcTextSize(targetLabels[i]);
                listDl->AddText(
                    { rowMin.x + 8.f, rowMin.y + (rowH - textSz.y) * 0.5f },
                    ImGui::ColorConvertFloat4ToU32(textCol), targetLabels[i]);

                ImGui::PopID();
            }

            ImGui::Unindent(6.f);
            ImGui::EndChild();

            ImGui::Spacing();
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 12.f, 10.f });
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 12.f, 6.f });
            if (ImGui::Button(XS("Copy##color_picker"), { 96.f, 28.f }))
            {
                FormatColorHex(hexBuf.data(), hexBuf.size(), pickerColor);
                ImGui::SetClipboardText(hexBuf.data());
            }
            ImGui::SameLine();
            if (ImGui::Button(XS("Close##color_picker"), { 96.f, 28.f }))
                ImGui::CloseCurrentPopup();
            ImGui::PopStyleVar(2);
            ImGui::Dummy({ 0.f, 2.f });
            ImGui::EndPopup();
        }

        ImGui::PopID();
        ImGui::SetCursorScreenPos(origin);
        ImGui::Dummy({ innerWidth, rowHeight });
        return rowHeight;
    }

    float DragFloatRow(const char* id, const char* label, float* value, float speed,
        const char* fmt, const ImVec2& origin, float innerWidth, bool bright = false)
    {
        GuiBindConfigFloat(id, value);

        const float ctrlH = ImGui::GetFrameHeight();
        const float rowH = ctrlH + 8.f;
        const float ctrlW = ImClamp(innerWidth * 0.55f, 140.f, innerWidth - 80.f);
        const float ctrlX = ImMax(origin.x + innerWidth - ctrlW, origin.x + 90.f);

        ImGui::SetCursorScreenPos({ origin.x, origin.y + (rowH - ImGui::GetFontSize()) * 0.5f });
        ImGui::PushStyleColor(ImGuiCol_Text, bright ? ImVec4(1, 1, 1, .92f) : ImVec4(1, 1, 1, .64f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImGui::SetCursorScreenPos({ ctrlX, origin.y + (rowH - ctrlH) * 0.5f });
        ImGui::PushItemWidth(ctrlW);
        ImGui::DragFloat((std::string("##") + id).c_str(), value, speed, 0, 0, fmt ? fmt : "%.2f");
        ImGui::PopItemWidth();

        return rowH;
    }

    float DragIntRow(const char* id, const char* label, int* value, float speed,
        const char* fmt, const ImVec2& origin, float innerWidth, bool bright = false)
    {
        GuiBindConfigInt(id, value);

        const float ctrlH = ImGui::GetFrameHeight();
        const float rowH = ctrlH + 8.f;
        const float ctrlW = ImClamp(innerWidth * 0.55f, 140.f, innerWidth - 80.f);
        const float ctrlX = ImMax(origin.x + innerWidth - ctrlW, origin.x + 90.f);

        ImGui::SetCursorScreenPos({ origin.x, origin.y + (rowH - ImGui::GetFontSize()) * 0.5f });
        ImGui::PushStyleColor(ImGuiCol_Text, bright ? ImVec4(1, 1, 1, .92f) : ImVec4(1, 1, 1, .64f));
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImGui::SetCursorScreenPos({ ctrlX, origin.y + (rowH - ctrlH) * 0.5f });
        ImGui::PushItemWidth(ctrlW);
        ImGui::DragInt((std::string("##") + id).c_str(), value, speed, 0, 0, fmt ? fmt : "%d");
        ImGui::PopItemWidth();

        return rowH;
    }

    void Atmosphere_UpdateBody(void* __this)
    {
        if (!self || !__this) return;

        if (!self->Active())
        {
            if (self->HasRestorePending())
                self->RestoreOriginals();
            return;
        }

        if (!self->EnsureInitialized()) return;

        auto* env = UnityUtils::EnviroSky_get_Instance();

        if (env && env->Components)   self->ApplySunMoonOverride(env);
        if (env && env->skySettings)  self->ApplyStarSettings(env->skySettings);
        if (env && env->lightSettings) self->ApplyLightSettings(env->lightSettings);

        if (env && env->cloudsSettings)
        {
            auto* clouds = reinterpret_cast<EnviroCloudSettings*>(env->cloudsSettings);
            const auto* colors = self->GetCloudColors();

            EnviroConfigColor* targets[] = {
                &clouds->cloudLightFrontColor,      &clouds->cloudLightBackColor,
                &clouds->cloudDarkFrontColor,       &clouds->cloudDarkBackColor,
                &clouds->effCloudLightFrontColorConfig, &clouds->effCloudLightBackColorConfig,
                &clouds->effCloudDarkFrontColorConfig,  &clouds->effCloudDarkBackColorConfig,
                &clouds->cloudLitToneColorConfig,   &clouds->cloudRimToneColorConfig,
                &clouds->cloudDarkToneColorConfig,  &clouds->cloudBackLitColorConfig,
            };

            for (int i = 0; i < AtmosphereFeature::kCloudColorCount; ++i)
                ApplyConfig(*targets[i], colors[i]);
        }

        UnityUtils::QualitySettings_set_shadows(self->GetShadowIndex());
        self->ApplyGrassOverride(self->GetNoGrass());
        UnityUtils::RenderSettings_set_fog(!self->GetNoFog());

        const auto ambient = self->GetAmbientColor();
        const auto equator = self->GetEquatorColor();
        const float ambInt = self->GetAmbientIntensity();
        const float eqInt = self->GetEquatorIntensity();

        UnityUtils::RenderSettings_set_ambientGroundColor({
            ambient.x * ambInt, ambient.y * ambInt, ambient.z * ambInt, ambient.w });
        UnityUtils::RenderSettings_set_ambientEquatorColor({
            equator.x * eqInt, equator.y * eqInt, equator.z * eqInt, equator.w });
    }

    UPDATE_HOOK(Atmosphere)
    {
        Atmosphere_UpdateBody(__this);
    }

} 

AtmosphereFeature::AtmosphereFeature()
    : Feature({ "card_atmosphere", XS("Atmosphere"), XS("Shape the environment mood"), "NONE", FeatureTab::Visual })
{
    self = this;
}

void AtmosphereFeature::OnEnable()
{
    hasOriginals_ = false;
    hasOriginalStars_ = false;
    hasOriginalLight_ = false;
    hasOriginalLightExtras_ = false;
    hasOriginalDayNightLerpSpeed_ = false;
    capturedSkySettings_ = nullptr;
    capturedLightSettings_ = nullptr;
    restorePending_ = false;
    grassOverrideActive_ = false;
    pendingInit_ = true;
}

void AtmosphereFeature::ApplyGrassOverride(bool noGrass)
{
    if (noGrass)
    {
        UnityUtils::MiHoYoVegetationManager_SetRuntimeQualityLevel(-1);
        grassOverrideActive_ = true;
    }
    else if (grassOverrideActive_)
    {
        UnityUtils::MiHoYoVegetationManager_SetRuntimeQualityLevel(originalGrassQuality_);
        grassOverrideActive_ = false;
    }
}

void AtmosphereFeature::ApplySunMoonOverride(EnviroSky* env)
{
    if (!env || !env->Components) return;
    if (env->Components->Sun)  UnityUtils::GameObject_SetActive(env->Components->Sun, !sunDisabled_);
    if (env->Components->Moon) UnityUtils::GameObject_SetActive(env->Components->Moon, !moonDisabled_);
}

void AtmosphereFeature::ApplyStarSettings(EnviroSkySettings* sky)
{
    if (!sky) return;

    if (capturedSkySettings_ != sky)
    {
        capturedSkySettings_ = sky;
        hasOriginalStars_ = false;
    }

    if (!hasOriginalStars_)
    {
        SaveConfig(sky->starsScintillation, originalStarsScintillation_);
        SaveConfig(sky->starsBrightness, originalStarsBrightness_);
        SaveConfig(sky->starsDensity, originalStarsDensity_);

        starsScintillation_ = sky->starsScintillation.inputValue;
        starsBrightness_ = sky->starsBrightness.inputValue;
        starsDensity_ = sky->starsDensity.inputValue;
        hasOriginalStars_ = true;
    }

    ApplyConfig(sky->starsScintillation, starsScintillation_);
    ApplyConfig(sky->starsBrightness, starsBrightness_);
    ApplyConfig(sky->starsDensity, starsDensity_);
}

void AtmosphereFeature::ApplyLightSettings(EnviroLightSettings* light)
{
    if (!light) return;

    if (capturedLightSettings_ != light)
    {
        capturedLightSettings_ = light;
        hasOriginalLight_ = false;
        hasOriginalLightExtras_ = false;
        hasOriginalDayNightLerpSpeed_ = false;
    }

    if (!hasOriginalLight_)
    {
        SaveConfig(light->mainLightColor, originalMainLightColor_);
        SaveConfig(light->mainLightIntensity, originalMainLightIntensity_);
        SaveConfig(light->ambientSkyColor, originalAmbientSkyColor_);
        SaveConfig(light->ambientEquatorColor, originalAmbientEquatorColor_);
        SaveConfig(light->ambientGroundColor, originalAmbientGroundColor_);

        mainLightColor_ = ColorToVec4(light->mainLightColor.inputColor);
        mainLightIntensity_ = light->mainLightIntensity.inputValue;
        ambientSkyColor_ = ColorToVec4(light->ambientSkyColor.inputColor);
        ambientEquatorColor_ = ColorToVec4(light->ambientEquatorColor.inputColor);
        ambientGroundColor_ = ColorToVec4(light->ambientGroundColor.inputColor);
        hasOriginalLight_ = true;
    }

    ApplyConfig(light->mainLightColor, mainLightColor_);
    ApplyConfig(light->mainLightIntensity, mainLightIntensity_);
    ApplyConfig(light->ambientSkyColor, ambientSkyColor_);
    ApplyConfig(light->ambientEquatorColor, ambientEquatorColor_);
    ApplyConfig(light->ambientGroundColor, ambientGroundColor_);

    auto* base = reinterpret_cast<uint8_t*>(light);

    if (!hasOriginalLightExtras_)
    {
        for (int i = 0; i < kLightExtraFloatCount; ++i)
        {
            auto* cfg = reinterpret_cast<EnviroConfigFloat*>(base + kLightExtraFloatDefs[i].offset);
            SaveConfig(*cfg, originalLightExtraFloats_[i]);
            lightExtraFloatValues_[i] = cfg->inputValue;
        }
        for (int i = 0; i < kLightExtraColorCount; ++i)
        {
            auto* cfg = reinterpret_cast<EnviroConfigColor*>(base + kLightExtraColorDefs[i].offset);
            SaveConfig(*cfg, originalLightExtraColors_[i]);
            lightExtraColorValues_[i] = ColorToVec4(cfg->inputColor);
        }
        hasOriginalLightExtras_ = true;
    }

    for (int i = 0; i < kLightExtraFloatCount; ++i)
        ApplyConfig(*reinterpret_cast<EnviroConfigFloat*>(base + kLightExtraFloatDefs[i].offset),
            lightExtraFloatValues_[i]);
    for (int i = 0; i < kLightExtraColorCount; ++i)
        ApplyConfig(*reinterpret_cast<EnviroConfigColor*>(base + kLightExtraColorDefs[i].offset),
            lightExtraColorValues_[i]);

    if (!hasOriginalDayNightLerpSpeed_)
    {
        originalDayNightLerpSpeed_ = light->dayNightLerpSpeed;
        hasOriginalDayNightLerpSpeed_ = true;
    }
    light->dayNightLerpSpeed = dayNightLerpSpeed_;
}

bool AtmosphereFeature::EnsureInitialized()
{
    if (hasOriginals_)  return true;
    if (!pendingInit_)  return false;

    originalShadowIndex_ = UnityUtils::QualitySettings_get_shadows();
    originalFog_ = UnityUtils::RenderSettings_get_fog();
    originalAmbientGround_ = ColorToVec4(UnityUtils::RenderSettings_get_ambientGroundColor());
    originalGrassQuality_ = UnityUtils::MiHoYoVegetationManager_GetRuntimeQualityLevel();

    hasOriginals_ = true;
    pendingInit_ = false;
    return true;
}

float AtmosphereFeature::OnGUI(const ImVec2& detailStart, float width)
{
    static const char* shadowModes[] = { XS("Off"), XS("Hard"), XS("Soft") };
    static const char* pages[] = { XS("Enviro"), XS("Sky") };

    static const char* cloudLabels[] = {
        XS("Light Front"),     XS("Light Back"),
        XS("Dark Front"),      XS("Dark Back"),
        XS("Effect Light Front"), XS("Effect Light Back"),
        XS("Effect Dark Front"),  XS("Effect Dark Back"),
        XS("Lit Tone"),        XS("Rim Tone"),
        XS("Dark Tone"),       XS("Back Lit"),
    };
    static const char* worldLabels[] = {
        XS("Main Light"),   XS("Ambient Sky"),
        XS("Ambient Equator"), XS("Ambient Ground"),
        XS("Water Shallow"),   XS("Water Deep"),
    };

    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiDropdownRow("atmo_page", XS("Mode"), pages,
        IM_ARRAYSIZE(pages), uiMode_, ImVec2(x, y), width) + 8.f;

    if (uiMode_ == 0)
    {
        y += GuiDropdownRow("atmo_shadow_mode", XS("Shadow"), shadowModes,
            IM_ARRAYSIZE(shadowModes), shadowIndex_, ImVec2(x, y), width) + 8.f;

        y += GuiColorButtonRow("atmo_ambient_color", XS("Ambient"), ambientColor_, ImVec2(x, y), width) + 6.f;
        y += GuiSliderRow("atmo_ambient_intensity", XS("Ambient Intensity"),
            &ambientIntensity_, 0.f, 6.f, "%.2f", ImVec2(x, y), width, true) + 6.f;

        y += GuiColorButtonRow("atmo_equator_color", XS("Equator"), equatorColor_, ImVec2(x, y), width) + 6.f;
        y += GuiSliderRow("atmo_equator_intensity", XS("Equator Intensity"),
            &equatorIntensity_, 0.f, 6.f, "%.2f", ImVec2(x, y), width, true) + 6.f;

        y += GuiToggleTextRow("atmo_no_fog", XS("No Fog"),
            XS("Enabled"), noFog_, ImVec2(x, y), width) + 6.f;
        y += GuiToggleTextRow("atmo_no_grass", XS("No Grass"),
            XS("Enabled"), noGrass_, ImVec2(x, y), width) + 6.f;
        y += GuiToggleTextRow("atmo_hold", XS("Key"),
            XS("Hold"), hold_, ImVec2(x, y), width) + 6.f;
    }
    else
    {
        y += GuiToggleTextRow("sky_sun_disabled", XS("Sun Disabled"),
            XS("Enabled"), sunDisabled_, ImVec2(x, y), width) + 6.f;
        y += GuiToggleTextRow("sky_moon_disabled", XS("Moon Disabled"),
            XS("Enabled"), moonDisabled_, ImVec2(x, y), width) + 8.f;

        y += GuiDetailRowText(XS("Stars"), "", ImVec2(x, y), width, false) + 4.f;
        y += DragFloatRow("sky_stars_scintillation", XS("Scintillation"),
            &starsScintillation_, 1.f, "%.2f", ImVec2(x, y), width, true) + 6.f;
        y += DragFloatRow("sky_stars_brightness", XS("Brightness"),
            &starsBrightness_, 1.f, "%.2f", ImVec2(x, y), width, true) + 6.f;
        y += DragFloatRow("sky_stars_density", XS("Density"),
            &starsDensity_, 1.f, "%.2f", ImVec2(x, y), width, true) + 8.f;

        y += DragFloatRow("light_main_intensity", XS("Main Intensity"),
            &mainLightIntensity_, 0.1f, "%.2f", ImVec2(x, y), width, true) + 8.f;

        std::array<ImVec4, 6> worldColors = {
            mainLightColor_, ambientSkyColor_, ambientEquatorColor_,
            ambientGroundColor_, lightExtraColorValues_[0], lightExtraColorValues_[1],
        };
        y += CloudColorPickerRow("sky_world_colors", XS("Worlds Colors"), worldPickerColor_,
            worldColors.data(), static_cast<int>(worldColors.size()), worldLabels,
            ImVec2(x, y), width) + 8.f;

        mainLightColor_ = worldColors[0];
        ambientSkyColor_ = worldColors[1];
        ambientEquatorColor_ = worldColors[2];
        ambientGroundColor_ = worldColors[3];
        lightExtraColorValues_[0] = worldColors[4];
        lightExtraColorValues_[1] = worldColors[5];

        y += GuiDetailRowText(XS("Light Extras"), "", ImVec2(x, y), width, false) + 4.f;
        y += DragFloatRow("light_day_night_lerp", XS("Lerp Speed"),
            &dayNightLerpSpeed_, 0.01f, "%.3f", ImVec2(x, y), width, true) + 6.f;

        for (int i = 0; i < kLightExtraFloatCount; ++i)
        {
            char id[64];
            _snprintf_s(id, sizeof(id), _TRUNCATE, "light_extra_f_%d", i);
            y += DragFloatRow(id, kLightExtraFloatDefs[i].label,
                &lightExtraFloatValues_[i], 0.1f, "%.2f", ImVec2(x, y), width, true) + 6.f;
        }

        y += CloudColorPickerRow("sky_clouds", XS("Cloud Colors"), cloudPickerColor_,
            cloudColors_.data(), kCloudColorCount, cloudLabels, ImVec2(x, y), width) + 6.f;
    }

    return (y - detailStart.y) + 12.f;
}

void AtmosphereFeature::OnShutdown()
{
    restorePending_ = true;
    pendingInit_ = false;
}

void AtmosphereFeature::RestoreOriginals()
{
    if (!restorePending_) return;
    pendingInit_ = false;

    auto* env = UnityUtils::EnviroSky_get_Instance();

    if (hasOriginalStars_ && env && env->skySettings)
    {
        RestoreConfig(originalStarsScintillation_, env->skySettings->starsScintillation);
        RestoreConfig(originalStarsBrightness_, env->skySettings->starsBrightness);
        RestoreConfig(originalStarsDensity_, env->skySettings->starsDensity);
        hasOriginalStars_ = false;
    }

    if (hasOriginalLight_ && env && env->lightSettings)
    {
        RestoreConfig(originalMainLightColor_, env->lightSettings->mainLightColor);
        RestoreConfig(originalMainLightIntensity_, env->lightSettings->mainLightIntensity);
        RestoreConfig(originalAmbientSkyColor_, env->lightSettings->ambientSkyColor);
        RestoreConfig(originalAmbientEquatorColor_, env->lightSettings->ambientEquatorColor);
        RestoreConfig(originalAmbientGroundColor_, env->lightSettings->ambientGroundColor);
    }

    if (hasOriginalLightExtras_ && env && env->lightSettings)
    {
        auto* base = reinterpret_cast<uint8_t*>(env->lightSettings);
        for (int i = 0; i < kLightExtraFloatCount; ++i)
            RestoreConfig(originalLightExtraFloats_[i],
                *reinterpret_cast<EnviroConfigFloat*>(base + kLightExtraFloatDefs[i].offset));
        for (int i = 0; i < kLightExtraColorCount; ++i)
            RestoreConfig(originalLightExtraColors_[i],
                *reinterpret_cast<EnviroConfigColor*>(base + kLightExtraColorDefs[i].offset));
        hasOriginalLightExtras_ = false;
    }

    if (hasOriginalDayNightLerpSpeed_ && env && env->lightSettings)
    {
        env->lightSettings->dayNightLerpSpeed = originalDayNightLerpSpeed_;
        hasOriginalDayNightLerpSpeed_ = false;
    }

    const bool canRestoreEquator = hasOriginalLight_;
    hasOriginalLight_ = false;

    if (grassOverrideActive_)
    {
        UnityUtils::MiHoYoVegetationManager_SetRuntimeQualityLevel(originalGrassQuality_);
        grassOverrideActive_ = false;
    }

    if (hasOriginals_)
    {
        UnityUtils::QualitySettings_set_shadows(originalShadowIndex_);
        UnityUtils::RenderSettings_set_fog(originalFog_);
        UnityUtils::RenderSettings_set_ambientGroundColor(Vec4ToColor(originalAmbientGround_));

        if (canRestoreEquator)
            UnityUtils::RenderSettings_set_ambientEquatorColor(originalAmbientEquatorColor_.inputColor);

        hasOriginals_ = false;
    }

    capturedSkySettings_ = nullptr;
    capturedLightSettings_ = nullptr;
    restorePending_ = false;
}