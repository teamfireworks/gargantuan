#include "gargantuan.core/datatypes/TweenInfo.hpp"
#include "gargantuan.runtime/ScriptEngine.hpp"
#include "gargantuan.runtime/StackValue.hpp"
#include <cstdint>
#include <lua.h>
#include <lualib.h>

namespace gargantuan {
	int LibTweenInfo_new(lua_State *L) {
		float time = luaL_optnumber(L, 1, 1.0f);
		Enums::EasingStyle style = CheckStackValue<Enums::EasingStyle>(L, 2);
		Enums::EasingDirection direction = CheckStackValue<Enums::EasingDirection>(L, 3);
		int32_t repeatCount = luaL_optinteger(L, 4, 0);
		bool reverses = luaL_optboolean(L, 5, false);
		float delayTime = luaL_optnumber(L, 6, 0.0f);

		TweenInfo tweeninfo = {
			time,
			style,
			direction,
			repeatCount,
			reverses,
			delayTime,
		};

		StackValue<TweenInfo>::Push(L, tweeninfo);
		return 1;
	}

	luaL_Reg LibTweenInfo[]{{"new", LibTweenInfo_new}, {nullptr, nullptr}};

	int OpenLibTweenInfo(lua_State *L) {
		luaL_register(L, "TweenInfo", LibTweenInfo);
		return 0;
	}
}
