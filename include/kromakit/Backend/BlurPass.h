#pragma once

#include <nanovg.h>

class BlurPass {
public:
	void SetStandardDeviation(float standardDeviation);
	float GetStandardDeviation() const;

	bool Apply(
		NVGcontext* context,
		int sourceImage,
		int destinationImage
	) const;

private:
	float standardDeviation_ = 12.0f;
};
