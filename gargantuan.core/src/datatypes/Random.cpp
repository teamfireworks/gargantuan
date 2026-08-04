#include "gargantuan.core/datatypes/Random.hpp"
#include "gargantuan.core/datatypes/Vector3.hpp"
#include "gargantuan.runtime/Userdata.hpp"
#include "gargantuan.runtime/UserdataTag.hpp"

#include <SDL3/SDL.h>
#include <glm/ext/scalar_constants.hpp>
#include <lua.h>
#include <lualib.h>
#include <random>
#include <utility>

namespace gargantuan {
	G_USERDATA_IMPL(
		Random,
		.Tag = UserdataTag::Random,
		.Type = "Random",
		.Methods = {
			{"Clone", Method::fromMember<&Random::Clone>()},
			{"NextInteger", Method::fromMember<&Random::NextInteger>()},
			{"NextUnitVector", Method::fromMember<&Random::NextUnitVector>()},
			{"NextNumber", Method{&Random::LNextNumber}},
			{"Shuffle", Method{&Random::LShuffle}},
		}
	)

	Random::Random() : Generator(SDL_GetTicksNS()) {}
	Random::Random(int64_t seed) : Generator(seed) {}

	Random Random::Clone() const {
		return *this;
	}

	double Random::NextNumber(double min, double max) {
		// It's safe to do this without swapping for min < max as LNextNumber
		// does it for us.
		return std::uniform_real_distribution<double>(min, max)(Generator);
	}

	int Random::NextInteger(int min, int max) {
		if (max < min) std::swap(min, max);
		return std::uniform_int_distribution<int>(min, max)(Generator);
	}

	glm::vec3 Random::NextUnitVector() {
		// Marsaglia's method
		double z = NextNumber(-1.0, 1.0);
		double angle = NextNumber(0.0, 2.0 * glm::pi<double>());
		double radius = glm::sqrt(1.0 - z * z);
		return glm::vec3(radius * glm::cos(angle), radius * glm::sin(angle), z);
	}

	int Random::LNextNumber(lua_State *L, Random *self) {
		if (lua_isnoneornil(L, 2)) {
			lua_pushnumber(L, self->NextNumber());
			return 1;
		}

		double min = luaL_checknumber(L, 2);
		double max = luaL_checknumber(L, 3);
		if (min > max) std::swap(min, max);
		lua_pushnumber(L, self->NextNumber(min, max));
		return 1;
	}

	int Random::LShuffle(lua_State *L, Random *self) {
		// Fisher-Yates over the array portion of the table, shuffled in place
		luaL_checktype(L, 2, LUA_TTABLE);

		int length = lua_objlen(L, 2);
		for (int i = length; i > 1; i--) {
			int j = self->NextInteger(1, i);
			if (i == j) {
				continue;
			}

			lua_rawgeti(L, 2, i);
			lua_rawgeti(L, 2, j);
			lua_rawseti(L, 2, i);
			lua_rawseti(L, 2, j);
		}

		return 0;
	}
}
