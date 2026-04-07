#include "CustomEffect.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Logger/Logger.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

#include <algorithm>
#include <string>

static CustomEffectFeature* self{ nullptr };
CustomEffectFeature::CustomEffectFeature()
    : Feature({ "card_effect", XS("Custom Effect"), XS(":p"), "NONE", FeatureTab::Visual })
{
    self = this;
}

void (*EffectManager_AttachProxyWithEffect_Orig)(void*, void*, void*) = nullptr;
void EffectManager_AttachProxyWithEffect_Hook(void* effectManager, void* monoEffect, void* proxyHandlePtr)
{
    EffectManager_AttachProxyWithEffect_Orig(effectManager, monoEffect, proxyHandlePtr);

    if (!monoEffect) return;

    auto* mono = reinterpret_cast<MonoEffect*>(monoEffect);
    if (!mono->effectName || mono->effectName->length <= 0) return;

    const auto name = OtherUtils::Utf16ToUtf8(mono->effectName->chars, mono->effectName->length);
    if (!name.empty()) Logger::log_info(name);
}

void (*MonoEffect_ShowRenders_Orig)(void*, bool) = nullptr;
void MonoEffect_ShowRenders_Hook(void* monoEffect, bool enable)
{
    MonoEffect_ShowRenders_Orig(monoEffect, enable);

    if (!monoEffect)return;

    auto* mono = reinterpret_cast<MonoEffect*>(monoEffect);
    if (!mono->effectName || mono->effectName->length <= 0)return;

    const auto name = OtherUtils::Utf16ToUtf8(mono->effectName->chars, mono->effectName->length);
    if (!name.empty())Logger::log_info(name);
}

float CustomEffectFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    bool confirmed = false;
    y += GuiInputConfirmRow("effect_changer", XS("Effect"),
        effectName_.data(), effectName_.size(), confirmed, ImVec2(x, y), width);

    if (confirmed)
    {
        const std::string effectName = effectName_.data();
        RunOnUnityThread([effectName] {
            if (effectName.empty())
                return;

            auto* localAvatar = OtherUtils::AvatarManager();
            auto* mgr = OtherUtils::EffectManager();
            if (!localAvatar || !mgr)
                return;

            auto* effectStr = UnityUtils::PtrToStringAnsi((void*)effectName.c_str());
            if (!effectStr)
                return;

            const auto pos = UnityUtils::GetRelativePosition_Entity(localAvatar);
            Quaternion rot{}; rot.w = 1.f;
            EffectUtils::CreateUnindexedEntityEffect(
                mgr, localAvatar, effectStr,
                pos.zero() ? NullNullable<Vector3>() : MakeNullable(pos),
                MakeNullable(rot),
                MakeNullable(Vector3{ 1.f, 1.f, 1.f }),
                NullNullable<Vector3>(),
                localAvatar, nullptr,
                NullNullable<Vector3>(),
                false, false, effectStr, false,
                NullNullable<OLOHHCPGJLF>()
            );
        });
    }

    y += 6.f;
    return (y - detailStart.y) + 12.f;
}

void CustomEffectFeature::OnDraw() {}

void CustomEffectFeature::OnEnable()
{
    if (!effectName_[0])
        return;

    const std::string effectName = effectName_.data();
    RunOnUnityThread([effectName] {
        if (!self || !self->Active() || effectName.empty())
            return;

        auto* localAvatar = OtherUtils::AvatarManager();
        auto* mgr = OtherUtils::EffectManager();
        if (!localAvatar || !mgr)
            return;

        auto* effectStr = UnityUtils::PtrToStringAnsi((void*)effectName.c_str());
        if (!effectStr)
            return;

        const auto pos = UnityUtils::GetRelativePosition_Entity(localAvatar);
        Quaternion rot{}; rot.w = 1.f;
        EffectUtils::CreateUnindexedEntityEffect(
            mgr, localAvatar, effectStr,
            pos.zero() ? NullNullable<Vector3>() : MakeNullable(pos),
            MakeNullable(rot),
            MakeNullable(Vector3{ 1.f, 1.f, 1.f }),
            NullNullable<Vector3>(),
            localAvatar, nullptr,
            NullNullable<Vector3>(),
            false, false, effectStr, false,
            NullNullable<OLOHHCPGJLF>()
        );
    });
}

void CustomEffectFeature::OnShutdown()
{
    SetEnabled(false);
}
