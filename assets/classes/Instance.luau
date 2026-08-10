const c, t, p = require("@tools/classes").unpack()

return c({
	name = "Instance",
	description = "Abstract class for all instances which can be part of the DataModel",
	since = "0.1.0",
	root = true,
	abstract = true,

	properties = {
		Archivable = {
			description = "Whether this instance can be archived through clones and saves",
			type = t.Bool,
			unmodified = "false",
		},
		ClassName = {
			description = "The class this instance belongs to",
			type = t.String,
			unmodified = "CLASS_DEFINITION.ClassName",
			overrideRead = "GetClassName",
		},
		Destroyed = {
			description = "Whether this instance has been destroyed",
			type = t.Bool,
			unmodified = "false",
		},
		Name = {
			description = "Non-unique identifier of the instance",
			type = t.String,
			unmodified = "CLASS_DEFINITION.ClassName",
		},
		Parent = {
			description = "Hierarchical parent of the instance, instances are unparented on destruction",
			type = t.Optional(t.Instance()),
			unmodified = "std::nullopt",
			overrideRead = "GetParent",
			overrideWrite = "SetParent",
		},
	},

	signals = {
		AncestryChanged = {
			description = "Fired after the Parent property of this or ancestor instances is changed",
			args = { t.Instance(), t.Instance() },
		},
		ChildAdded = {
			description = "Fired after an instance is parented to this instance",
			args = { t.Instance() },
		},
		ChildRemoved = {
			description = "Fired after an instance is unparented from this instance",
			args = { t.Instance() },
		},
		DescendantAdded = {
			description = "Fired after a descendant instance is added to this instance",
			args = { t.Instance() },
		},
		DescendantRemoved = {
			description = "Fired after a descendant instance is removed from this instance",
			args = { t.Instance() },
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
		Destroy = {
			description = "Destroys this instance",
		},

		IsPropertyModified = {
			description = "Whether the named property has been modified",
			params = { p("propertyName", t.String) },
			returns = t.Bool,
		},
		ResetPropertyToDefault = {
			description = "Resets the named property to the base unmodified value",
			params = { p("propertyName", t.String) },
		},
		GetPropertyChangedSignal = {
			description = "Returns a signal that is fired when the specified property is changed",
			params = { p("propertyName", t.String) },
			returns = t.Signal(),
		},

		GetChildren = {
			description = "Returns all direct children of this instance",
			returns = t.Array(t.Instance()),
		},
		ClearAllChildren = {
			description = "Unparents all direct children of this instance",
		},
		-- WaitForChild = {
		-- 	description = "Yields until the named child is parented to this instance",
		-- 	asLuau = true,
		-- 	luauSignature = "(self: Instance, name: string, timeout: number?) -> Instance",
		-- },
		FindFirstChild = {
			description = "Returns the first direct child that matches the specified name",
			params = { p("name", t.String), p("recursive", t.Optional(t.Bool)) },
			returns = t.Instance(),
		},
		FindFirstChildOfClass = {
			description = "Returns the first direct child that exactly matches the specified class",
			params = { p("className", t.String), p("recursive", t.Optional(t.Bool)) },
			returns = t.Instance(),
		},
		FindFirstChildWhichIsA = {
			description = "Returns the first direct child that derives from the specified class",
			params = { p("className", t.String), p("recursive", t.Optional(t.Bool)) },
			returns = t.Instance(),
		},
		-- BindChildren = {
		-- 	description = "Calls the specified callback on existing and future direct children. Returns a disconnector.",
		-- 	params = { p("callback", t.Function(p("instance", t.Instance()))()) },
		-- 	returns = t.Function()(),
		-- },

		-- IsDescendantOf = {
		-- 	description = "Whether this instance is a descendant of the specified ancestor",
		-- 	params = { p("ancestor", t.Instance()) },
		-- 	returns = t.Bool,
		-- },
		GetDescendants = {
			description = "Returns all descendants below this instance",
			returns = t.Array(t.Instance()),
		},
		FindFirstDescendant = {
			description = "Returns the first descendant that matches the specified name",
			params = { p("name", t.String) },
			returns = t.Instance(),
		},
		FindFirstDescendantOfClass = {
			description = "Returns the first descendant that exactly matches the specified class",
			params = { p("className", t.String) },
			returns = t.Instance(),
		},
		FindFirstDescendantWhichIsA = {
			description = "Returns the first descendant that derives from the specified class",
			params = { p("className", t.String) },
			returns = t.Instance(),
		},
		-- BindDescendants = {
		-- 	description = "Calls the specified callback on existing and future descendants. Returns a disconnector.",
		-- 	params = { p("callback", t.Function(p("instance", t.Instance()))()) },
		-- 	returns = t.Function()(),
		-- },

		-- IsAncestorOf = {
		-- 	description = "Whether this instance is an ancestor of the specified descendant",
		-- 	params = { p("descendant", t.Instance()) },
		-- 	returns = t.Bool,
		-- },
		-- GetAncestors = {
		-- 	description = "Returns all ancestors above this instance",
		-- 	returns = t.Array(t.Instance()),
		-- },
		FindFirstAncestor = {
			description = "Returns the first ancestor that matches the specified name",
			params = { p("name", t.String) },
			returns = t.Instance(),
		},
		FindFirstAncestorOfClass = {
			description = "Returns the first ancestor that exactly matches the specified class",
			params = { p("className", t.String) },
			returns = t.Instance(),
		},
		FindFirstAncestorWhichIsA = {
			description = "Returns the first ancestor that derives from the specified class",
			params = { p("className", t.String) },
			returns = t.Instance(),
		},
	},
})
