const c, t, p = require("@tools/classes").unpack()

return c({
	name = "GlslSourceContainer",
	description = "Abstract class for compiling shader bytecode",
	abstract = true,
	since = "0.1.0",

	properties = {
		Source = {
			description = "GLSL source code of the shader",
			type = t.String,
			unmodified = "std::string()",
		},
	},
})
