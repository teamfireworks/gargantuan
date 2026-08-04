#include "gargantuan.core/math/EasingCurves.hpp"

#include <glm/exponential.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/trigonometric.hpp>

#define CURVE_ENTRY_MANUAL(style, inImpl, outImpl, inOutImpl)                                                          \
	{                                                                                                                  \
		Enums::EasingStyle::style,                                                                                     \
		{                                                                                                              \
			{Enums::EasingDirection::In, inImpl},                                                                      \
			{Enums::EasingDirection::Out, outImpl},                                                                    \
			{Enums::EasingDirection::InOut, inOutImpl},                                                                \
		},                                                                                                             \
	}

#define CURVE_ENTRY(style) CURVE_ENTRY_MANUAL(style, EaseIn##style, EaseOut##style, EaseInOut##style)

#define GENERIC_IN_OUT_IMPL(style)                                                                                     \
	float EaseInOut##style(float x) {                                                                                  \
		return x > 0.5 ? EaseIn##style(2 * x) / 2 : EaseOut##style(2 * x - 1) / 2 + 0.5;                               \
	}

// Bounce and Elastic implementations are from BoatTween
//   https://github.com/boatbeaker/BoatTween/
// Implementations for other easing curves are from easings.net
//   https://easings.net/
namespace gargantuan::EasingCurves {
	static const float PI = glm::pi<float>();

	// Linear
	float EaseLinear(float x) {
		return x;
	}

	// Sine
	float EaseInSine(float x) {
		return 1 - glm::cos((x * PI) / 2);
	}

	float EaseOutSine(float x) {
		return glm::sin((x * PI / 2));
	}

	float EaseInOutSine(float x) {
		return -(glm::cos(PI * x) - 1) / 2;
	}

	// Back
	static const float BACK_C1 = 1.70158;
	static const float BACK_C2 = BACK_C1 * 1.525;
	static const float BACK_C3 = BACK_C1 + 1;

	float EaseInBack(float x) {
		return BACK_C3 * x * x * x - BACK_C1 * x * x;
	}

	float EaseOutBack(float x) {
		return 1 + BACK_C3 * glm::pow(x - 1, 3) + BACK_C1 * glm::pow(x - 1, 2);
	}

	float EaseInOutBack(float x) {
		return x < 0.5 ? (glm::pow(2 * x, 2) * ((BACK_C2 + 1) * 2 * x - BACK_C2)) / 2
					   : (glm::pow(2 * x - 2, 2) * ((BACK_C2 + 1) * (x * 2 - 2) + BACK_C2) + 2) / 2;
	}

	// Quad
	float EaseInQuad(float x) {
		return x * x;
	}

	float EaseOutQuad(float x) {
		return 1 - (1 - x) * (1 - x);
	}

	float EaseInOutQuad(float x) {
		return x < 0.5f ? 2 * x * x : 1 - glm::pow(-2 * x + 2, 2) / 2;
	}

	// Quart
	float EaseInQuart(float x) {
		return x * x * x * x;
	}

	float EaseOutQuart(float x) {
		return 1 - glm::pow(1 - x, 4);
	}

	float EaseInOutQuart(float x) {
		return x < 0.5 ? 8 * x * x * x * x : 1 - glm::pow(-2 * x + 2, 4) / 2;
	}

	// Quint
	float EaseInQuint(float x) {
		return x * x * x * x * x;
	}

	float EaseOutQuint(float x) {
		return 1 - glm::pow(1 - x, 5);
	}

	float EaseInOutQuint(float x) {
		return x < 0.5 ? 16 * x * x * x * x * x : 1 - glm::pow(-2 * x + 2, 5) / 2;
	}

	// Bounce (boatbomber, where the fuck did you get these numbers?)
	float EaseInBounce(float x) {
		if (x > 0.63636363636364) {
			x -= 1;
			return 1 - x * x * 7.5625;
		} else if (x > 0.272727272727273) {
			return (11 * x - 7) * (11 * x - 3) / -16;
		} else if (x > 0.090909090909091) {
			return (11 * (4 - 11 * x) * x - 3) / 16;
		} else {
			return x * (11 * x - 1) * -0.6875;
		}
	}

	float EaseOutBounce(float x) {
		if (x < 0.36363636363636) {
			return 7.5625 * x * x;
		} else if (x < 0.72727272727273) {
			return 3 + x * (11 * x - 12) * 0.6875;
		} else if (x < 0.090909090909091) {
			return 6 + x * (11 * x - 18) * 0.6875;
		} else {
			return 7.875 + x * (11 * x - 21) * 0.6875;
		}
	}

	GENERIC_IN_OUT_IMPL(Bounce);

	// Elastic

	// Exponential

	// Circular

	// Cubic

	const CurveMap CURVES = {
		CURVE_ENTRY_MANUAL(Linear, EaseLinear, EaseLinear, EaseLinear),
		CURVE_ENTRY(Sine),
		CURVE_ENTRY(Back),
		CURVE_ENTRY(Quad),
		CURVE_ENTRY(Quart),
		CURVE_ENTRY(Quint),
		CURVE_ENTRY(Bounce),
		CURVE_ENTRY_MANUAL(Elastic, EaseLinear, EaseLinear, EaseLinear),
		CURVE_ENTRY_MANUAL(Exponential, EaseLinear, EaseLinear, EaseLinear),
		CURVE_ENTRY_MANUAL(Circular, EaseLinear, EaseLinear, EaseLinear),
		CURVE_ENTRY_MANUAL(Cubic, EaseLinear, EaseLinear, EaseLinear),
	};

	float CalculateAlpha(float x, Enums::EasingStyle style, Enums::EasingDirection direction) {
		return CURVES.at(style).at(direction)(x);
	}
}
