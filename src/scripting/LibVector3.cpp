#include "gargantuan/datatypes/Vector3.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"

#include <glm/glm.hpp>
#include <lua.h>
#include <lualib.h>

namespace gargantuan {
	int Vector3_Abs(lua_State *L) {
		glm::vec3 self = StackValue<glm::vec3>::From(L, 1);
		StackValue<glm::vec3>::Push(L, glm::abs(self));
		return 1;
	}

	int Vector3_Ceil(lua_State *L) {
		glm::vec3 self = StackValue<glm::vec3>::From(L, 1);
		StackValue<glm::vec3>::Push(L, glm::ceil(self));
		return 1;
	}

	int Vector3_Floor(lua_State *L) {
		glm::vec3 self = StackValue<glm::vec3>::From(L, 1);
		StackValue<glm::vec3>::Push(L, glm::floor(self));
		return 1;
	}

	int Vector3_Sign(lua_State *L) {
		glm::vec3 self = StackValue<glm::vec3>::From(L, 1);
		StackValue<glm::vec3>::Push(L, glm::sign(self));
		return 1;
	}

	int Vector3_Cross(lua_State *L) {
		glm::vec3 self = StackValue<glm::vec3>::From(L, 1);
		glm::vec3 other = StackValue<glm::vec3>::From(L, 2);
		StackValue<glm::vec3>::Push(L, glm::cross(self, other));
		return 1;
	}

	int Vector3_Angle(lua_State *L) {
		glm::vec3 self = StackValue<glm::vec3>::From(L, 1);
		glm::vec3 other = StackValue<glm::vec3>::From(L, 2);
		double dot = glm::dot(glm::normalize(self), glm::normalize(other));
		lua_pushnumber(L, glm::acos(glm::clamp<double>(dot, -1, 1)));
		return 1;
	}

	int Vector3_Dot(lua_State *L) {
		glm::vec3 self = StackValue<glm::vec3>::From(L, 1);
		glm::vec3 other = StackValue<glm::vec3>::From(L, 2);
		lua_pushnumber(L, glm::dot(self, other));
		return 1;
	}

	int Vector3_FuzzyEq(lua_State *L) {
		glm::vec3 self = StackValue<glm::vec3>::From(L, 1);
		glm::vec3 other = StackValue<glm::vec3>::From(L, 2);
		float epsilon = luaL_optnumber(L, 3, 1e-5);
		lua_pushboolean(
			L,
			glm::abs(self.x - other.x) <= epsilon && glm::abs(self.y - other.y) <= epsilon &&
				glm::abs(self.z - other.z) <= epsilon
		);
		return 1;
	}

	int Vector3_Lerp(lua_State *L) {
		glm::vec3 self = StackValue<glm::vec3>::From(L, 1);
		glm::vec3 other = StackValue<glm::vec3>::From(L, 2);
		double alpha = luaL_checknumber(L, 3);
		lua_pushvector(
			L,
			self.x + (other.x - self.x) * alpha,
			self.y + (other.y - self.y) * alpha,
			self.z + (other.z - self.z) * alpha
		);
		return 1;
	}

	int Vector3_Min(lua_State *L) {
		glm::vec3 self = StackValue<glm::vec3>::From(L, 1);
		glm::vec3 other = StackValue<glm::vec3>::From(L, 2);
		StackValue<glm::vec3>::Push(L, glm::min(self, other));
		return 1;
	}

	int Vector3_Max(lua_State *L) {
		glm::vec3 self = StackValue<glm::vec3>::From(L, 1);
		glm::vec3 other = StackValue<glm::vec3>::From(L, 2);
		StackValue<glm::vec3>::Push(L, glm::max(self, other));
		return 1;
	}

	std::unordered_map<std::string_view, std::function<int(lua_State *)>> Vector3_methods = {
		{"Abs", Vector3_Abs},
		{"Ceil", Vector3_Ceil},
		{"Floor", Vector3_Floor},
		{"Sign", Vector3_Sign},
		{"Cross", Vector3_Cross},
		{"Angle", Vector3_Angle},
		{"Dot", Vector3_Dot},
		{"FuzzyEq", Vector3_FuzzyEq},
		{"Lerp", Vector3_Lerp},
		{"Min", Vector3_Min},
		{"Max", Vector3_Max},
	};

	int Vector3_namecall(lua_State *L) {
		const char *key = lua_namecallatom(L, nullptr);
		if (auto it = Vector3_methods.find(key); it != Vector3_methods.end()) {
			return it->second(L);
		} else {
			luaL_errorL(L, "Unknown Vector3 method named %s", key);
			return 0;
		}
	}

	int Vector3_index(lua_State *L) {
		glm::vec3 self = StackValue<glm::vec3>::From(L, 1);
		const char *key = luaL_checkstring(L, 2);

		if (std::strcmp(key, "Magnitude") == 0) {
			lua_pushnumber(L, glm::length(self));
			return 1;
		} else if (std::strcmp(key, "Unit") == 0) {
			StackValue<glm::vec3>::Push(L, self.x == 0 && self.y == 0 && self.z == 0 ? self : glm::normalize(self));
			return 1;
		}

		return 0;
	}

	void Vector3_register(lua_State *L) {
		lua_newtable(L);

		lua_pushliteral(L, "Vector3");
		lua_setfield(L, -2, "__type");

		lua_pushcfunction(L, Vector3_index, "Vector3.__index");
		lua_setfield(L, -2, "__index");

		lua_pushcfunction(L, Vector3_namecall, "__namecall");
		lua_setfield(L, -2, "__namecall");

		// lua_setreadonly(L, -1, true);

		// push a dummy vector to get it's metatable
		lua_pushvector(L, 0.0f, 0.0f, 0.0f);
		// push a copy of it's metatable
		lua_pushvalue(L, -2);
		// set the metatable of that to our table
		lua_setmetatable(L, -2);
		// pop the dummy vector
		lua_pop(L, 1);
	}

	int LibVector3_new(lua_State *L) {
		lua_pushvector(L, luaL_optnumber(L, 1, 0.0f), luaL_optnumber(L, 2, 0.0f), luaL_optnumber(L, 3, 0.0f));
		return 1;
	}

	void LibVector3_register(lua_State *L) {
		lua_createtable(L, 0, 6);

		lua_pushvector(L, 0, 0, 0);
		lua_setfield(L, -2, "zero");

		lua_pushvector(L, 1, 1, 1);
		lua_setfield(L, -2, "one");

		lua_pushvector(L, 1, 0, 0);
		lua_setfield(L, -2, "xAxis");

		lua_pushvector(L, 0, 1, 0);
		lua_setfield(L, -2, "yAxis");

		lua_pushvector(L, 0, 0, 1);
		lua_setfield(L, -2, "zAxis");

		lua_pushcfunction(L, LibVector3_new, "Vector3.new");
		lua_setfield(L, -2, "new");

		// lua_setreadonly(L, -1, true);
		lua_setglobal(L, "Vector3");
	}

	int OpenLibVector3(lua_State *L) {
		Vector3_register(L);
		LibVector3_register(L);
		return 0;
	}
} // namespace gargantuan
