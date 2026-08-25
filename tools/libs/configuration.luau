-- FIXME: blocked on Seal supporting .config.luau

return {
	buildDirectory = "build",
	binaryBasename = "gargantuan",

	declgen = {
		datamodel = {
			inputDir = "tools/decls/datamodel",
			codegenScript = "tools/libs/codegen/generateInstanceClass",
			macroOutputDir = "src/datamodel/generated",
			registerOutput = "src/registration/RegisterDatamodel.cpp",
		},
		datatypes = {
			inputDir = "tools/decls/datatypes",
			codegenScript = "tools/libs/codegen/generateDataType",
			macroOutputDir = "src/datatypes/generated",
			registerOutput = "src/registration/RegisterDatatypes.cpp",
		},
	},
}
