#pragma once

#include "gargantuan.runtime/Userdata.hpp"

#include <cstdint>
#include <ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <lua.h>
#include <random>

namespace gargantuan {
	G_USERDATA_DECL(
		Random,

		Random();
		Random(std::int64_t seed);

		Random Clone() const;
		int NextInteger(int min, int max);
		double NextNumber(double min = 0.0f, double max = 1.0f);
		glm::vec3 NextUnitVector();
		static int LNextNumber(lua_State *L, Random *self);
		static int LShuffle(lua_State *L, Random *self);

		private : std::mt19937_64 Generator;
		std::uint64_t NextBits();
	);
}
