#pragma once

#include "gargantuan/reflection/Enums.hpp"
#include <functional>
#include <unordered_map>

namespace gargantuan {
	G_ENUM(EasingDirection, In, Out, InOut);
	G_ENUM(EasingStyle, Linear, Sine, Back, Quad, Quart, Quint, Bounce, Elastic, Exponential, Circular, Cubic);

	namespace EasingCurves {
		typedef std::function<float(float x)> Curve;
		typedef std::unordered_map<Enums::EasingStyle, std::unordered_map<Enums::EasingDirection, Curve>> CurveMap;
		extern const CurveMap CURVES;

		float CalculateAlpha(
			float x,
			Enums::EasingStyle style = Enums::EasingStyle::Linear,
			Enums::EasingDirection direction = Enums::EasingDirection::Out
		);
	}
};
