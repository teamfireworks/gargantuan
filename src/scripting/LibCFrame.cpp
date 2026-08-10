// NOTE: Need StackValue<glm::vec3> implementation
#include "gargantuan/datatypes/Vector3.hpp" // IWYU pragma: keep

#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"
#include "gargantuan/scripting/StackValue.hpp"

#include <lualib.h>

namespace gargantuan {
	int LibCFrame_new(lua_State *L) {
		int argumentCount = lua_gettop(L);

		if (argumentCount == 0) {
			StackValue<CFrame>::Push(L, CFrame());
			return 1;
		} else if (argumentCount == 1) {
			glm::vec3 pos = CheckStackValue<glm::vec3>(L, 1);
			StackValue<CFrame>::Push(L, CFrame(pos));
			return 1;
		} else if (argumentCount == 2) {
			glm::vec3 pos = CheckStackValue<glm::vec3>(L, 1);
			glm::vec3 target = CheckStackValue<glm::vec3>(L, 2);

			StackValue<CFrame>::Push(L, CFrame(pos, target));
			return 1;
		} else if (argumentCount == 3) {
			float x = luaL_checknumber(L, 1);
			float y = luaL_checknumber(L, 2);
			float z = luaL_checknumber(L, 3);
			StackValue<CFrame>::Push(L, CFrame(x, y, z));
			return 1;
		}

		luaL_error(L, "unsupported constructor");
		return 0;
	}

	int LibCFrame_Angles(lua_State *L) {
		float rx = luaL_checknumber(L, 1);
		float ry = luaL_checknumber(L, 2);
		float rz = luaL_checknumber(L, 3);
		StackValue<CFrame>::Push(L, CFrame::Angles(rx, ry, rz));
		return 1;
	}

	luaL_Reg LibCFrame[] = {
		{"new", LibCFrame_new},
		{"Angles", LibCFrame_Angles},
		{nullptr, nullptr},
	};

	int OpenLibCFrame(lua_State *L) {
		luaL_register(L, "CFrame", LibCFrame);
		return 0;
	}
} // namespace gargantuan
