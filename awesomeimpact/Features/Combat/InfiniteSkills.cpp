#include "InfiniteSkills.h"
#include "../../GUI/gui.h"
#include "../../Utils/test02.h"

static InfiniteSkillsFeature* self { nullptr };
InfiniteSkillsFeature::InfiniteSkillsFeature()
    : Feature({ "card_inf_skills", XS("Infinite Skills"), XS("Keep abilities off CD"), XS("NONE"), FeatureTab::Combat })
{
    self = this;
}

void (*TickSkillCd_Orig)(void*, float);
void TickSkillCd_Hook(void* __this, float deltaTime)
{
    if (!self || !self->Active()) return TickSkillCd_Orig(__this, deltaTime);
    if (self->GetInfE()) return TickSkillCd_Orig(__this, 0.9f);
    TickSkillCd_Orig(__this, deltaTime);
}
bool (*IsEnergyMax_Orig)(void* __this);
bool IsEnergyMax_Hook(void* __this)
{
    if (!self || !self->Active()) return IsEnergyMax_Orig(__this);
    if (self->GetInfQ()) return true;
    return IsEnergyMax_Orig(__this);
}

float InfiniteSkillsFeature::OnGUI(const ImVec2& detailStart, float width)
{
    float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;
    float checkboxSpacing = ImGui::GetFrameHeight() + 6.f;

    GuiCustomCheckbox("skill_q", XS("Infinite Q"), infSkillQ_, ImVec2(x, y));
    y += checkboxSpacing;
    GuiCustomCheckbox("skill_e", XS("Infinite E"), infSkillE_, ImVec2(x, y));
    y += checkboxSpacing;

    float holdRow = GuiToggleTextRow("inf_skills_hold", XS("Key"), XS("Hold"), hold_, ImVec2(x, y), width);
    y += holdRow + 6.f;
    return (y - detailStart.y) + 12.f;
}

void InfiniteSkillsFeature::OnShutdown() 
{
    self->SetEnabled(false);
}