#include "KeepCamera.h"
#include "../../GUI/gui.h"
#include "../../globals.h"

KeepCameraFeature* self{ nullptr };
KeepCameraFeature::KeepCameraFeature()
    : Feature({ "card_keep_camera", XS("Keep Camera"), XS("Prevent profile menu camera shift"), "NONE", FeatureTab::Misc })
{
    self = this;
}

namespace
{
   

}

void KeepCamera_OnFlushPre(void* outputCamera)
{
   
}

void KeepCamera_OnFlushPost(void* outputCamera)
{
   
}

void KeepCamera_OnProfileMenuOpen()
{
    
}

void KeepCamera_OnProfileMenuClose()
{
    
}

float KeepCameraFeature::OnGUI(const ImVec2& detailStart, float width)
{
    float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

    ImGui::SetCursorScreenPos(ImVec2(x, y));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 0.65f));
    ImGui::TextUnformatted(XS(":3"));
    ImGui::PopStyleColor();

    y += ImGui::GetFontSize() + 12.f;
    return (y - detailStart.y) + 12.f;
}

void KeepCameraFeature::OnShutdown()
{
   
    self->SetEnabled(false);
}
