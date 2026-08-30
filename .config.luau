return {
	luau = {
		languagemode = "strict",
		lint = { ["*"] = true },
		aliases = {
			batteries = ".lute/batteries",
			std = "~/.lute/typedefs/1.0.0/std",
			lint = "~/.lute/typedefs/1.0.0/lint",
			lute = "~/.lute/typedefs/1.0.0/lute",

			lest = "./.lest/core",
			codegen = "./codegen",
		},
	},

	gargantuan = {
		rootDir = true,
		buildDir = "build",

		codegenTargets = {
			datatypes = {
				schemaDir = "codegen/datatypes",
				outputDir = "src/datatypes/generated",
				registryPath = "src/datatypes/generated/gg_RegisterDatatypes.cpp",
			},
			instances = {
				schemaDir = "codegen/instances",
				outputDir = "src/instances/generated",
				registryPath = "src/instances/generated/gg_RegisterInstances.cpp",
			},
		},
	},
}
