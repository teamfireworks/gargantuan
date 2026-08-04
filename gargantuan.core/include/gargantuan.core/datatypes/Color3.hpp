#pragma once

#include "gargantuan.runtime/Userdata.hpp"

#include <glm/glm.hpp>
#include <lua.h>

namespace gargantuan {
	G_USERDATA_DECL(
		Color3,

		float R = 0.0f;
		float G = 0.0f;
		float B = 0.0f;

		Color3();
		Color3(float r, float g, float b);

		static Color3 fromRGB(float r, float g, float b);
		static Color3 fromHSV(float h, float s, float v);
		static Color3 fromHex(std::string_view hex);

		operator glm::vec3() const { return {R, G, B}; }
	);
} // namespace gargantuan
