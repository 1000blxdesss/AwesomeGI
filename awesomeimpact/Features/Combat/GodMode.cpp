#include "GodMode.h"
#include "../../GUI/gui.h"
#include "../../globals.h"
#include "../../Utils/HookRegistry.h"
#include "../../Utils/test02.h"

static GodModeFeature* self{ nullptr };
GodModeFeature::GodModeFeature()
    : Feature({ "card_godmode", XS("Godmode"), XS("Immune to damage"), XS("NONE"), FeatureTab::Combat })
{
    self = this;
}
bool (*CheckTargetAttackable_Orig)(void*, void*, bool) = nullptr;
bool CheckTargetAttackable_Hook(void* attacker, void* target, bool checkBackstage)
{
	bool out = false;
	for (auto fn : GetCheckTargetAttackableHandlers()) {
		if (fn(attacker, target, checkBackstage, out))
			return out;
	}

	return CheckTargetAttackable_Orig ? CheckTargetAttackable_Orig(attacker, target, checkBackstage) : true;
}

_(GodMode_CheckTargetAttackable)
{
	if (!self || !self->Active())
		return false;

	if (!attacker || !target) {
		outResult = CheckTargetAttackable_Orig ? CheckTargetAttackable_Orig(attacker, target, checkBackstage) : true;
		return true;
	}

	auto* playerEntity = OtherUtils::AvatarManager();
	if (!playerEntity)
		return false;

	const auto playerRuntimeID = *reinterpret_cast<uint32_t*>(
		reinterpret_cast<uintptr_t>(playerEntity) + offsets::MoleMole::BaseEntity::runtimeID);
	const auto targetRuntimeID = *reinterpret_cast<uint32_t*>(
		reinterpret_cast<uintptr_t>(target) + offsets::MoleMole::BaseEntity::runtimeID);

	if (targetRuntimeID == playerRuntimeID) {
		outResult = false;
		return true;
	}

	return false;
}

void (*NotifyLandVelocity_Orig)(void*, Vector3, float) = nullptr;
void NotifyLandVelocity_Hook(void* __this, Vector3 velocity, float reachMaxDownVelocityTime)
{
	if (!self || !self->Active() || !self->GetNoFall() || !__this) return NotifyLandVelocity_Orig(__this, velocity, reachMaxDownVelocityTime);

	if (velocity.y < -15.f) // дамаг будет получен примерно от  -25.f
	{
		velocity.y = (std::rand() % 2 == 0) ? -9.21f : -9.48f;
		reachMaxDownVelocityTime = 0.0f;
	}

	NotifyLandVelocity_Orig(__this, velocity, reachMaxDownVelocityTime);
}

float GodModeFeature::OnGUI(const ImVec2& detailStart, float width)
{
    float x = detailStart.x + 20.f;
    float y = detailStart.y + 12.f;

	float holdRow = GuiToggleTextRow("godmode_nofall", XS("NoFall"), XS("Enabled"), noFall_, ImVec2(x, y), width);
	y += holdRow + 6.f;
    holdRow = GuiToggleTextRow("godmode_hold", XS("Key"), XS("Hold"), hold_, ImVec2(x, y), width);
    y += holdRow + 6.f;
   

    return (y - detailStart.y) + 12.f;
}

void GodModeFeature::OnShutdown()
{
}
