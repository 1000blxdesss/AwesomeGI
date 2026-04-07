#include "BoobaScale.h"
#include "../../GUI/gui.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

static BoobaScaleFeature* self{ nullptr };
BoobaScaleFeature::BoobaScaleFeature()
    : Feature({ "card_booba_scale", XS("Booba Scale"), XS("Adjust chest size"), "NONE", FeatureTab::Misc })
{
    self = this;
}

namespace {

    HOOK_INSTALL(HumanoidMoveFSM_LateTick, Booba_LateTick)
    {
        HOOK_RET_ORIG(HumanoidMoveFSM_LateTick);
        void* __this = HOOK_ARG(HumanoidMoveFSM_LateTick, 0);

        if (!self || !self->Active()) return ret_orig();

        auto* root = BoneUtils::GetLocalAvatarRootTransform();
        if (root)
        {
            const float s = self->GetScale();
            BoneUtils::ForEachBone(root, XS("+Breast"), false, [s](const BoneUtils::BoneData& b) {
                UnityUtils::Transform_set_localScale(b.transform, { s, s, s });
                });
        }

        return ret_orig();
    }

}

float BoobaScaleFeature::OnGUI(const ImVec2& detailStart, float width)
{
    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiSliderRow("booba_scale", XS("Scale"),
        &scale_, 1.f, 10.f, "%.1f", ImVec2(x, y), width, true) + 6.f;

    return (y - detailStart.y) + 12.f;
}

void BoobaScaleFeature::OnShutdown()
{
    SetEnabled(false);
}