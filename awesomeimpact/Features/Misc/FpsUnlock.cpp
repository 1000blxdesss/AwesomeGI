#include "FpsUnlock.h"
#include "../../GUI/gui.h"
#include "../../Utils/HookRegistry.h"
#include "../../Logger/Logger.h"
#include "../../Utils/test02.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <cstdio>

FpsUnlockFeature* self{ nullptr };
FpsUnlockFeature::FpsUnlockFeature()
    : Feature({ "card_fps_unlock", XS("FPS Unlock"), XS("Raise frame cap"), "NONE", FeatureTab::Misc })
{
    self = this;
}

namespace
{
    inline float Clamp01(float v)
    {
        return (v < 0.0f) ? 0.0f : ((v > 1.0f) ? 1.0f : v);
    }

    inline Color LerpColor(const Color& a, const Color& b, float t)
    {
        t = Clamp01(t);
        return {
            a.r + (b.r - a.r) * t,
            a.g + (b.g - a.g) * t,
            a.b + (b.b - a.b) * t,
            a.a + (b.a - a.a) * t
        };
    }

    inline void ColorToHex(const Color& c, char out[10])
    {
        const int r = (int)(Clamp01(c.r) * 255.0f + 0.5f);
        const int g = (int)(Clamp01(c.g) * 255.0f + 0.5f);
        const int b = (int)(Clamp01(c.b) * 255.0f + 0.5f);
        const int a = (int)(Clamp01(c.a) * 255.0f + 0.5f);
        _snprintf_s(out, 10, _TRUNCATE, "#%02X%02X%02X%02X", r, g, b, a);
    }

    inline std::string BuildUidGradientText(float t)
    {
        static const std::string kText = XS("AwesomeGI");
        static const Color kRed = { 1.0f, 0.16f, 0.16f, 1.0f };
        static const Color kWhite = { 1.0f, 1.0f, 1.0f, 1.0f };

        const float wave = fmodf(t * 0.65f, 1.0f);
        std::string out;
        out.reserve(kText.size() * 28);

        for (size_t i = 0; i < kText.size(); ++i)
        {
            const float u = (kText.size() <= 1) ? 0.0f : ((float)i / (float)(kText.size() - 1));

            float mix = u;
            if (i < 7)
                mix *= 0.55f;
            else
                mix = 0.70f + (u - 0.70f) * 1.35f;
            mix = Clamp01(mix);

            float d = fabsf(u - wave);
            d = (std::min)(d, 1.0f - d);
            const float glow = expf(-(d * d) * 90.0f);

            Color col = LerpColor(kRed, kWhite, mix);
            col = LerpColor(col, kWhite, glow * 0.75f);
            col.a = 1.0f;

            char hex[10]{};
            ColorToHex(col, hex);
            out += XS("<color=");
            out += hex;
            out += XS(">");
            out.push_back(kText[i]);
            out += XS("</color>");
        }

        return out;
    }

    inline void TickUidBranding()
    {
        void* go = UnityUtils::GameObject_Find(UnityUtils::PtrToStringAnsi((void*)XS("TxtUID")));
        if (!go)
            return;

        void* txt = UnityUtils::GameObject_GetComponentByName(go, UnityUtils::PtrToStringAnsi((void*)XS("Text")));
        if (!txt)
            return;

        static void* s_lastTxt = nullptr;
        static int s_baseFontSize = 0;
        if (txt != s_lastTxt)
        {
            s_lastTxt = txt;
            s_baseFontSize = UnityUtils::Text_get_fontSize(txt);
        }
        const int boostedSize = (s_baseFontSize > 0 ? s_baseFontSize + 12 : 42);
        UnityUtils::Text_set_fontSize(txt, boostedSize);
        if (void* textTransform = UnityUtils::Component_get_transform(txt))
            UnityUtils::Transform_set_localScale(textTransform, { 1.35f, 1.35f, 1.0f });

        const float t = (float)ImGui::GetTime();
        const std::string rich = BuildUidGradientText(t);
        UnityUtils::Text_set_text(txt, UnityUtils::PtrToStringAnsi((void*)rich.c_str()));

        const float flash = 0.92f + 0.08f * (0.5f + 0.5f * sinf(t * 7.5f));
        UnityUtils::Graphic_set_color(txt, { 1.0f, 1.0f, 1.0f, flash });
    }

    UPDATE_HOOK(FPS)
    {
        //TickUidBranding();

        if (!self || !self->Active())return;

        static int lastCap = -99999;
        const int cap = (int)self->GetLimit();
        if (cap != lastCap)
        {
            UnityUtils::Application_set_targetFrameRate(cap);
            lastCap = cap;
        }
    }
}

float FpsUnlockFeature::OnGUI(const ImVec2& detailStart, float width)
{
    float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiSliderRow("fps_unlock_value", XS("Limit"), &limit_, -1.f, 240.f, "%.0f", ImVec2(x, y), width, true);
    y += 10.f;

    float holdRow = GuiToggleTextRow("fps_unlock_hold", XS("Key"), XS("Hold"), hold_, ImVec2(x, y), width);
    y += holdRow + 6.f;

    return (y - detailStart.y) + 12.f;
}

void FpsUnlockFeature::OnEnable()
{
}

void FpsUnlockFeature::OnShutdown()
{
    self->SetEnabled(false);
}