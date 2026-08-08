const c, t, p = require("@tools/classes").unpack()

return c({
	name = "DirectoryLink",
	description = "Class that synchronizes a filesystem directory into the DataModel",
	since = "0.1.0",

	properties = {
		Path = {
			description = "Path of the directory to synchronize from",
			type = t.String,
			unmodified = '""',
		},
		IgnoreErrors = {
			description = "Whether to ignore errors when deserializing the directory's contents",
			type = t.Bool,
			unmodified = "false",
		},
		IgnoreUnknownDescendants = {
			description = "Whether to ignore unknown descendants that don't match the directory's contents",
			type = t.Bool,
			unmodified = "false",
		},
	},
})
