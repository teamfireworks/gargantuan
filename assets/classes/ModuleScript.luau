const c, t, p = require("@tools/classes").unpack()

return c({
	name = "ModuleScript",
	extends = "LuaSourceContainer",
	description = "Class that contains a Lua module which can be required at runtime",
	since = "0.1.0",
})
