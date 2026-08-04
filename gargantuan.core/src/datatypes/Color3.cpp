#include "gargantuan.core/datatypes/Color3.hpp"
#include "gargantuan.runtime/Userdata.hpp"
#include "gargantuan.runtime/UserdataTag.hpp"

#include <common.hpp>

namespace gargantuan {
	G_USERDATA_IMPL(
		Color3,
		.Tag = UserdataTag::Color3,
		.Type = "Color3",
		.Properties = {
			{"R", Color3::Property::fromReadonlyMember<&Color3::R>()},
			{"G", Color3::Property::fromReadonlyMember<&Color3::G>()},
			{"B", Color3::Property::fromReadonlyMember<&Color3::B>()}
		}
	);

	Color3::Color3() : R(0.0f), G(0.0f), B(0.0f) {};

	Color3::Color3(float r, float g, float b)
		: R(glm::clamp(r, 0.0f, 1.0f)), G(glm::clamp(g, 0.0f, 1.0f)), B(glm::clamp(b, 0.0f, 1.0f)) {}

	Color3 Color3::fromRGB(float r, float g, float b) {
		return Color3(r / 255.0f, g / 255.0f, b / 255.0f);
	};

	Color3 Color3::fromHSV(float h, float s, float v) {
		h = glm::mod(h, 1.0f);
		s = glm::clamp(s, 0.0f, 1.0f);
		v = glm::clamp(v, 0.0f, 1.0f);

		auto i = glm::floor(h * 6);
		auto f = h * 6 - i;
		auto p = v * (1 - s);
		auto q = v * (1 - f * s);
		auto t = v * (1 - (1 - f) * s);

		float r, g, b;
		i = glm::mod(i, 6.0f);

		return (i == 0)	  ? Color3(v, t, p)
			   : (i == 1) ? Color3(q, v, p)
			   : (i == 2) ? Color3(p, v, t)
			   : (i == 3) ? Color3(p, q, v)
			   : (i == 4) ? Color3(t, p, v)
						  : Color3(v, p, q);
	}
} // namespace gargantuan
