const i, t, p = require("@tools/inst").unpack()

return i.class({
	name = "Instance",
	description = "Base class for all instances which can be part of the DataModel",
	since = "0.1.0",

	properties = {
		Archivable = {
			type = t.Bool,
			description = "Whether this instance can be archived through clones and saves",
			unmodified = "false",
		},
		ClassName = {
			type = t.String,
			description = "The class this instance belongs to",
			unmodified = "CLASS_DEFINITION.ClassName",
			overrideRead = "GetClassName",
		},
		Name = {
			type = t.String,
			description = "Non-unique identifier of the instance",
			unmodified = "CLASS_DEFINITION.ClassName",
		},
		Parent = {
			type = t.Optional(t.Instance),
			description = "Hierarchical parent of the instance, instances are unparented on destruction",
			unmodified = "std::nullopt",
			overrideRead = "GetParent",
			overrideWrite = "SetParent",
		},
	},

	signals = {
		AncestryChanged = {
			description = "Fired after the Parent property of this or ancestor instances is changed",
			args = { t.Instance, t.Instance },
		},
		ChildAdded = {
			description = "Fired after an instance is parented to this instance",
			args = { t.Instance },
		},
		ChildRemoved = {
			description = "Fired after an instance is unparented from this instance",
			args = { t.Instance },
		},
		DescendantAdded = {
			description = "Fired after a descendant instance is added to this instance",
			args = { t.Instance },
		},
		DescendantRemoved = {
			description = "Fired after a descendant instance is removed from this instance",
			args = { t.Instance },
		},
		Destroying = {
			description = "Fired once when the instance is destroyed",
			args = {},
		},
	},

	methods = {
		IsA = {
			description = "Whether this instance derives from the specified class",
			params = { p("className", t.String) },
			returns = t.Bool,
		},
		GetFullName = {
			description = "Returns the instance's ancestry as a period-separated path",
			returns = t.String,
		},
		GetPropertyChangedSignal = {
			description = "Returns a signal that is fired when the specified property is changed",
			params = { p("propertyName", t.String) },
			returns = t.Signal,
		},

		GetChildren = {
			description = "Returns all direct children of this instance",
			returns = t.Array(t.Instance),
		},
		FindFirstChild = {
			description = "Returns the first direct child that matches the specified name",
			params = { p("name", t.String), p("recursive", t.Optional(t.Bool)) },
			returns = t.Instance,
		},
		FindFirstChildOfClass = {
			description = "Returns the first direct child that exactly matches the specified class",
			params = { p("className", t.String), p("recursive", t.Optional(t.Bool)) },
			returns = t.Instance,
		},
		FindFirstChildWhichIsA = {
			description = "Returns the first direct child that derives from the specified class",
			params = { p("className", t.String), p("recursive", t.Optional(t.Bool)) },
			returns = t.Instance,
		},

		IsDescendantOf = {
			description = "Whether this instance is a descendant of the specified ancestor",
			params = { p("ancestor", t.Instance) },
			returns = t.Bool,
		},
		GetDescendants = {
			description = "Returns all descendants below this instance",
			returns = t.Array(t.Instance),
		},
		FindFirstDescendant = {
			description = "Returns the first descendant that matches the specified name",
			params = { p("name", t.String) },
			returns = t.Instance,
		},
		FindFirstDescendantOfClass = {
			description = "Returns the first descendant that exactly matches the specified class",
			params = { p("className", t.String) },
			returns = t.Instance,
		},
		FindFirstDescendantWhichIsA = {
			description = "Returns the first descendant that derives from the specified class",
			params = { p("className", t.String) },
			returns = t.Instance,
		},

		IsAncestorOf = {
			description = "Whether this instance is an ancestor of the specified descendant",
			params = { p("descendant", t.Instance) },
			returns = t.Bool,
		},
		GetAncestors = {
			description = "Returns all ancestors above this instance",
			returns = t.Array(t.Instance),
		},
		FindFirstAncestor = {
			description = "Returns the first ancestor that matches the specified name",
			params = { p("name", t.String) },
			returns = t.Instance,
		},
		FindFirstAncestorOfClass = {
			description = "Returns the first ancestor that exactly matches the specified class",
			params = { p("className", t.String) },
			returns = t.Instance,
		},
		FindFirstAncestorWhichIsA = {
			description = "Returns the first ancestor that derives from the specified class",
			params = { p("className", t.String) },
			returns = t.Instance,
		},
	},
})
