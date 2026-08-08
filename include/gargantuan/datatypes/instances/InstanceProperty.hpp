#pragma once

#include "gargantuan/reflection/Enums.hpp"
#include <functional>
#include <string>

namespace gargantuan {
	G_ENUM(
		Permission,

		// Minimum of any script
		None,
		// Minimum of any plugin
		Plugin,
		// Minimum of the local development machine, unused in a shipped game
		LocalUser,

		// We skip over WritePlayer since it is irrelevant to Gargantuan;
		// games itself make player instances

		// Minimum of engine-level scripts
		EngineScript = 4,
		// Minimum of Gargantuan itself
		Engine,

#ifdef __NDEBUG__
		Debug = 0,
#else
		Debug,
#endif

		// This feature should never be used
		Never = 7
	);

	class Instance;

	class InstanceProperty {
		std::string Name{};
		bool Serializable{false};

		Enums::Permission ReadPermission = Enums::Permission::None;
		std::function<std::string()> GetReadType;
		std::function<std::any(Instance *self)> RawRead;
		std::function<int(lua_State *L, std::any value)> PushStack;

		Enums::Permission WritePermission = Enums::Permission::None;
		std::function<std::string()> GetWriteType;
		std::function<void(Instance *self, std::any value)> RawWrite;
		std::function<bool(lua_State *L, int idx)> IsStack;
		std::function<std::any(lua_State *L, int idx)> FromStack;
	};

	// todo: finish this
}
