#pragma once

#include "../FeatureBase.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include <array>
#include <vector>

struct EnviroComponents;
struct EnviroSky;

class AtmosphereFeature final : public Feature
{
public:
    AtmosphereFeature();
    void OnEnable() override;
    float OnGUI(const ImVec2& detailStart, float width) override;
    void OnShutdown() override;
    void ApplyGrassOverride(bool noGrass);
    void ApplySunMoonOverride(EnviroSky* env);
    void ApplyStarSettings(EnviroSkySettings* sky);
    void ApplyLightSettings(EnviroLightSettings* light);
    void RestoreOriginals();
    bool HasRestorePending() const { return restorePending_; }
    bool EnsureInitialized();
    int GetShadowIndex() const { return shadowIndex_; }
    bool GetNoFog() const { return noFog_;}
    bool GetNoGrass() const { return noGrass_; }
    ImVec4 GetAmbientColor() const { return ambientColor_; }
    ImVec4 GetEquatorColor() const { return equatorColor_; }
    float GetAmbientIntensity() const { return ambientIntensity_; }
    float GetEquatorIntensity() const { return equatorIntensity_; }
    static constexpr int kCloudColorCount = 12;
    const ImVec4* GetCloudColors() const { return cloudColors_.data(); }
    static constexpr int kLightExtraFloatCount = 12;
    static constexpr int kLightExtraColorCount = 2;

private:
    int shadowIndex_ = 0;
    ImVec4 ambientColor_ = ImVec4(0.01f, 0.01f, 0.01f, 1.0f);
    ImVec4 equatorColor_ = ImVec4(0.01f, 0.01f, 0.01f, 1.0f);
    float ambientIntensity_ = 0.0f;
    float equatorIntensity_ = 0.0f;
    int uiMode_ = 0;
    ImVec4 cloudPickerColor_ = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 worldPickerColor_ = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    std::array<ImVec4, kCloudColorCount> cloudColors_ = {
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // Light Front
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // Light Back
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // Dark Front
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // Dark Back
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // Eff Light Front
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // Eff Light Back
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // Eff Dark Front
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // Eff Dark Back
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // Lit Tone
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // Rim Tone
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // Dark Tone
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f)  // Back Lit
    };
    int originalShadowIndex_ = 0;
    bool originalFog_ = false;
    ImVec4 originalAmbientGround_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    bool hasOriginals_ = false;
    bool hasOriginalStars_ = false;
    EnviroSkySettings* capturedSkySettings_ = nullptr;
    EnviroConfigFloat originalStarsScintillation_{};
    EnviroConfigFloat originalStarsBrightness_{};
    EnviroConfigFloat originalStarsDensity_{};
    int originalGrassQuality_ = 0;
    bool hasOriginalLight_ = false;
    EnviroLightSettings* capturedLightSettings_ = nullptr;
    EnviroConfigColor originalMainLightColor_{};
    EnviroConfigFloat originalMainLightIntensity_{};
    EnviroConfigColor originalAmbientSkyColor_{};
    EnviroConfigColor originalAmbientEquatorColor_{};
    EnviroConfigColor originalAmbientGroundColor_{};
    bool grassOverrideActive_ = false;
    bool pendingInit_ = false;
    bool restorePending_ = false;
    bool hold_ = false;
    bool noFog_ = false;
    bool noGrass_ = false;
    bool sunDisabled_ = false;
    bool moonDisabled_ = false;
    float starsScintillation_ = 0.0f;
    float starsBrightness_ = 0.0f;
    float starsDensity_ = 0.0f;
    ImVec4 mainLightColor_ = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    float mainLightIntensity_ = 0.0f;
    ImVec4 ambientSkyColor_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 ambientEquatorColor_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 ambientGroundColor_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    bool hasOriginalLightExtras_ = false;
    std::array<float, kLightExtraFloatCount> lightExtraFloatValues_{};
    std::array<EnviroConfigFloat, kLightExtraFloatCount> originalLightExtraFloats_{};
    std::array<ImVec4, kLightExtraColorCount> lightExtraColorValues_{};
    std::array<EnviroConfigColor, kLightExtraColorCount> originalLightExtraColors_{};
    float dayNightLerpSpeed_ = 0.0f;
    float originalDayNightLerpSpeed_ = 0.0f;
    bool hasOriginalDayNightLerpSpeed_ = false;
};
