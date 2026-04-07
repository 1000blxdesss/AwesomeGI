#pragma once
#include "../FeatureBase.h"
class KillAuraFeature final : public Feature
{
public:
	KillAuraFeature();
	float OnGUI(const ImVec2& detailStart, float width) override;
	void OnDraw() override;
	void OnShutdown() override;
	void OnEnable() override;


	float GetRadius() const { return radius_; }
	float GetPrecent() const { return precent_; }
	float GetDelayMin() const { return attackDelayMin_; }
	float GetDelayMax() const { return attackDelayMax_; }
	bool IsDrawRadius() const { return drawRadius_; }

private:
	bool hold_ = false;
	bool drawRadius_ = true;
	float radius_ = 5.f;
	float precent_ = 100;
	float attackDelayMin_ = 0.f;
	float attackDelayMax_ = 15.f;
};


