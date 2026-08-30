local path = require("@std/path")

export type CodegenInput<Schema> = {
	schema: Schema,
	schemaPath: path.Pathlike,
	outputPath: path.Pathlike,
}

return nil
