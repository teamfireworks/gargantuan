#pragma once

#include "gargantuan/classes/generated/ModuleScript.hpp"

#include <lua.h>

namespace gargantuan {
	class ModuleScript : public LuaSourceContainer {
		I_ModuleScript;
	};
}
