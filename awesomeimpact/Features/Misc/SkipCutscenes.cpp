#include "SkipCutscenes.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#include <atomic>

static SkipCutscenesFeature* self{ nullptr };
SkipCutscenesFeature::SkipCutscenesFeature()
    : Feature({ "card_skip_cutscenes", XS("Skip Cutscenes"), XS("Skip ALL"), "NONE", FeatureTab::Misc })
{
    self = this;
}

namespace {
    std::atomic<void*> g_lastBridge{ nullptr };
}

void (*VideoBridge_AsyncPlayLike_Orig)(void*) = nullptr;
void VideoBridge_AsyncPlayLike_Hook(void* __this)
{
    g_loginDoorOverlayActive = false;
    g_lastBridge.store(__this);
    if (VideoBridge_AsyncPlayLike_Orig)VideoBridge_AsyncPlayLike_Orig(__this);
}

void (*CriwareMediaPlayer_OnStartPlay_Orig)(void*) = nullptr;
void CriwareMediaPlayer_OnStartPlay_Hook(void* __this)
{
    g_loginDoorOverlayActive = false;
    if (CriwareMediaPlayer_OnStartPlay_Orig)CriwareMediaPlayer_OnStartPlay_Orig(__this);

    if (!self || !self->Active() || !__this)
        return;

    UnityUtils::CriwareMediaPlayer_Skip(__this);

    auto* bridge = g_lastBridge.load();
    if (bridge)UnityUtils::IPlayNetworkVideo_Stop(bridge);
}

float SkipCutscenesFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    ImGui::SetCursorScreenPos({ x, y });
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.65f));
    ImGui::TextUnformatted(XS(":3"));
    ImGui::PopStyleColor();
    y += ImGui::GetFontSize() + 12.f;

    return (y - detailStart.y) + 12.f;
}

void SkipCutscenesFeature::OnShutdown()
{
    SetEnabled(false);
    g_lastBridge.store(nullptr);
}