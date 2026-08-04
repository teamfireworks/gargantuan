#include "gargantuan.core/classes/ModuleScript.hpp"
#include "gargantuan.core/reflection/InstanceClassRegistry.hpp"
#include "gargantuan.runtime/ScriptEngine.hpp"

#include <lua.h>

namespace gargantuan {
	G_INSTANCE_IMPL(ModuleScript, .Superclass = "LuaSourceContainer");
}
