#pragma once

#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/classes/LuaSourceContainer.hpp"

#include <lua.h>

namespace gargantuan {
	class ModuleScript : public LuaSourceContainer {
	  public:
		G_INSTANCE_DECL(ModuleScript);
	};
}
