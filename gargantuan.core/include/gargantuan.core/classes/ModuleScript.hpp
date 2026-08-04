#pragma once

#include "gargantuan/classes/LuaSourceContainer.hpp"
#include "gargantuan/datatypes/Instance.hpp"

#include <lua.h>

namespace gargantuan {
	class ModuleScript : public LuaSourceContainer {
	  public:
		G_INSTANCE_DECL(ModuleScript);
	};
}
