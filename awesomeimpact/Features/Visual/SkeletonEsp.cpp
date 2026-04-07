#include "SkeletonEsp.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/test02.h"

#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

static SkeletonEspFeature* self{ nullptr };
SkeletonEspFeature::SkeletonEspFeature()
    : Feature({ "card_skeleton", XS("Skeleton ESP"), XS("BOOO!! Skeleton!!!"), "NONE", FeatureTab::Visual })
{
    self = this;
}

float SkeletonEspFeature::OnGUI(const ImVec2& detailStart, float width)
{
    static const char* targets[] = { XS("Avatar"), XS("Brain") };

    const float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    y += GuiDropdownRow("skeleton_target", XS("Target"), targets,
        IM_ARRAYSIZE(targets), targetIndex_, ImVec2(x, y), width) + 10.f;

    y += GuiColorButtonRow("skeleton_bone_color", XS("Bones"),
        boneColor_, ImVec2(x, y), width) + 8.f;

    return (y - detailStart.y) + 12.f;
}

namespace {

    struct BoneLink { const char* a; const char* b; };

    const BoneLink kAvatarLinks[] = {
        // spine
        { XS("Bip001 Head"),   XS("Bip001 Neck")   },
        { XS("Bip001 Neck"),   XS("Bip001 Spine2") },
        { XS("Bip001 Spine2"), XS("Bip001 Spine1") },
        { XS("Bip001 Spine1"), XS("Bip001 Spine")  },
        { XS("Bip001 Spine"),  XS("Bip001 Pelvis") },
        // left arm
        { XS("Bip001 Spine2"),     XS("Bip001 L Clavicle") },
        { XS("Bip001 L Clavicle"), XS("Bip001 L UpperArm") },
        { XS("Bip001 L UpperArm"), XS("Bip001 L Forearm")  },
        { XS("Bip001 L Forearm"),  XS("Bip001 L Hand")     },
        // right arm
        { XS("Bip001 Spine2"),     XS("Bip001 R Clavicle") },
        { XS("Bip001 R Clavicle"), XS("Bip001 R UpperArm") },
        { XS("Bip001 R UpperArm"), XS("Bip001 R Forearm")  },
        { XS("Bip001 R Forearm"),  XS("Bip001 R Hand")     },
        // left leg
        { XS("Bip001 Pelvis"), XS("Bip001 L Thigh") },
        { XS("Bip001 L Thigh"), XS("Bip001 L Calf") },
        { XS("Bip001 L Calf"),  XS("Bip001 L Foot") },
        // right leg
        { XS("Bip001 Pelvis"), XS("Bip001 R Thigh") },
        { XS("Bip001 R Thigh"), XS("Bip001 R Calf") },
        { XS("Bip001 R Calf"),  XS("Bip001 R Foot") },
        // left fingers
        { XS("Bip001 L Hand"),       XS("Bip001 L Finger0")    }, { XS("Bip001 L Finger0"),    XS("Bip001 L Finger01")   },
        { XS("Bip001 L Finger01"),   XS("Bip001 L Finger02")   }, { XS("Bip001 L Finger02"),   XS("Bip001 L Finger0Nub") },
        { XS("Bip001 L Hand"),       XS("Bip001 L Finger1")    }, { XS("Bip001 L Finger1"),    XS("Bip001 L Finger11")   },
        { XS("Bip001 L Finger11"),   XS("Bip001 L Finger12")   }, { XS("Bip001 L Finger12"),   XS("Bip001 L Finger1Nub") },
        { XS("Bip001 L Hand"),       XS("Bip001 L Finger2")    }, { XS("Bip001 L Finger2"),    XS("Bip001 L Finger21")   },
        { XS("Bip001 L Finger21"),   XS("Bip001 L Finger22")   }, { XS("Bip001 L Finger22"),   XS("Bip001 L Finger2Nub") },
        { XS("Bip001 L Hand"),       XS("Bip001 L Finger3")    }, { XS("Bip001 L Finger3"),    XS("Bip001 L Finger31")   },
        { XS("Bip001 L Finger31"),   XS("Bip001 L Finger32")   }, { XS("Bip001 L Finger32"),   XS("Bip001 L Finger3Nub") },
        { XS("Bip001 L Hand"),       XS("Bip001 L Finger4")    }, { XS("Bip001 L Finger4"),    XS("Bip001 L Finger41")   },
        { XS("Bip001 L Finger41"),   XS("Bip001 L Finger42")   }, { XS("Bip001 L Finger42"),   XS("Bip001 L Finger4Nub") },
        // right fingers
        { XS("Bip001 R Hand"),       XS("Bip001 R Finger0")    }, { XS("Bip001 R Finger0"),    XS("Bip001 R Finger01")   },
        { XS("Bip001 R Finger01"),   XS("Bip001 R Finger02")   }, { XS("Bip001 R Finger02"),   XS("Bip001 R Finger0Nub") },
        { XS("Bip001 R Hand"),       XS("Bip001 R Finger1")    }, { XS("Bip001 R Finger1"),    XS("Bip001 R Finger11")   },
        { XS("Bip001 R Finger11"),   XS("Bip001 R Finger12")   }, { XS("Bip001 R Finger12"),   XS("Bip001 R Finger1Nub") },
        { XS("Bip001 R Hand"),       XS("Bip001 R Finger2")    }, { XS("Bip001 R Finger2"),    XS("Bip001 R Finger21")   },
        { XS("Bip001 R Finger21"),   XS("Bip001 R Finger22")   }, { XS("Bip001 R Finger22"),   XS("Bip001 R Finger2Nub") },
        { XS("Bip001 R Hand"),       XS("Bip001 R Finger3")    }, { XS("Bip001 R Finger3"),    XS("Bip001 R Finger31")   },
        { XS("Bip001 R Finger31"),   XS("Bip001 R Finger32")   }, { XS("Bip001 R Finger32"),   XS("Bip001 R Finger3Nub") },
        { XS("Bip001 R Hand"),       XS("Bip001 R Finger4")    }, { XS("Bip001 R Finger4"),    XS("Bip001 R Finger41")   },
        { XS("Bip001 R Finger41"),   XS("Bip001 R Finger42")   }, { XS("Bip001 R Finger42"),   XS("Bip001 R Finger4Nub") },
    };

    inline void BuildTransformMap(void* root, std::unordered_map<std::string, void*>& out)
    {
        out.clear();
        if (!root) return;

        std::vector<void*> stack;
        stack.push_back(root);

        while (!stack.empty())
        {
            auto* t = stack.back(); stack.pop_back();
            if (!t) continue;

            auto* go = UnityUtils::Component_get_gameObject(t);
            if (go)
            {
                auto* nameStr = UnityUtils::Object_get_name(go);
                if (nameStr && nameStr->length > 0)
                {
                    auto n = OtherUtils::Utf16ToUtf8(nameStr->chars, nameStr->length);
                    if (!n.empty()) out.emplace(std::move(n), t);
                }
            }

            const int cc = UnityUtils::Transform_get_childCount(t);
            if (cc <= 0 || cc > 4096) continue;
            for (int i = 0; i < cc; ++i)
                if (auto* child = UnityUtils::Transform_GetChild(t, i))
                    stack.push_back(child);
        }
    }

    inline void DrawLine3D(ImDrawList* dl, void* camera, float scrW, float scrH,
        const Vector3& a, const Vector3& b, ImU32 col, float thick)
    {
        const auto as = OtherUtils::WorldToScreenPointCorrected(camera, a);
        const auto bs = OtherUtils::WorldToScreenPointCorrected(camera, b);
        if (as.z <= 1.f || bs.z <= 1.f) return;
        if (!std::isfinite(as.x) || !std::isfinite(as.y) || !std::isfinite(bs.x) || !std::isfinite(bs.y)) return;

        const float mx = scrW * 0.5f, my = scrH * 0.5f;
        if (as.x < -mx || as.x > scrW + mx || as.y < -my || as.y > scrH + my) return;
        if (bs.x < -mx || bs.x > scrW + mx || bs.y < -my || bs.y > scrH + my) return;

        dl->AddLine({ as.x, scrH - as.y }, { bs.x, scrH - bs.y }, col, thick);
    }

}

void SkeletonEspFeature::OnDraw()
{
    if (!self || !self->Active()) return;

    auto* camera = UnityUtils::GetMainCamera();
    if (!camera) return;

    auto* dl = ImGui::GetBackgroundDrawList();
    if (!dl) return;

    const auto screenSize = ImGui::GetIO().DisplaySize;
    const ImU32 col = ImGui::ColorConvertFloat4ToU32(boneColor_);

    if (targetIndex_ == 0)
    {
        const auto localAvatar = OtherUtils::AvatarManager();
        if (!localAvatar) return;

        auto* rootObj = UnityUtils::BaseEntity_rootObject(localAvatar);
        if (!rootObj) return;

        auto* trName = UnityUtils::PtrToStringAnsi((void*)XS("Transform"));
        auto* rootTr = UnityUtils::GameObject_GetComponentByName(rootObj, trName);
        if (!rootTr) return;

        static std::unordered_map<std::string, void*> nameMap;
        static bool reserved = false;
        if (!reserved) { nameMap.reserve(4096); reserved = true; }

        BuildTransformMap(rootTr, nameMap);

        for (const auto& link : kAvatarLinks)
        {
            const auto itA = nameMap.find(link.a);
            const auto itB = nameMap.find(link.b);
            if (itA == nameMap.end() || itB == nameMap.end()) continue;

            const auto a = UnityUtils::Transform_get_position(itA->second);
            const auto b = UnityUtils::Transform_get_position(itB->second);
            if (a.zero() || b.zero()) continue;

            DrawLine3D(dl, camera, screenSize.x, screenSize.y, a, b, col, 2.f);
        }
    }

}

void SkeletonEspFeature::OnShutdown()
{
    SetEnabled(false);
}
