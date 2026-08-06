#include "gargantuan/classes/ModuleScript.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"

#include <lua.h>

namespace gargantuan {
	I_IMPL(ModuleScript, .Superclass = "LuaSourceContainer");
}
