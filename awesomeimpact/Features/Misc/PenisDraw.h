#pragma once
#include "../FeatureBase.h"
#include <array>

class PenisDrawFeature final : public Feature
{
public:
	std::array<char, 512> texturePath_{};
	bool textureApplied_ = false;

	PenisDrawFeature();
	float OnGUI(const ImVec2& detailStart, float width) override;
	void OnShutdown() override;
	float GetSize() const { return size_; }
	float GetThickness() const { return thickness_; }

private:
	

	bool hold_ = false;
	float size_ = 0.17f;
	float thickness_ = 0.05f;
};

