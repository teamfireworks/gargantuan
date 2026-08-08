#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"

#include <lualib.h>

namespace gargantuan {
	int LibInstance_new(lua_State *L) {
		auto className = luaL_checkstring(L, 1);

		auto classDefinition = InstanceClassRegistry::GetDefinitionByName(className);
		if (!classDefinition) {
			luaL_error(L, "Unknown instance class named %s", className);
			return 0;
		}

		auto constructor = classDefinition->Constructor;
		if (!constructor) {
			luaL_error(L, "Instance class %s cannot be constructed", className);
			return 0;
		}

		auto instance = constructor();
		StackValue<std::shared_ptr<Instance>>::Push(L, instance);
		return 1;
	};

	luaL_Reg LibInstance[] = {
		{"new", LibInstance_new},
		{nullptr, nullptr},
	};

	int OpenLibInstance(lua_State *L) {
		luaL_register(L, "Instance", LibInstance);
		return 0;
	}
} // namespace gargantuan
