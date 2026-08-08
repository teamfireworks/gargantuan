const c, t, p = require("@tools/classes").unpack()

return c({
	name = "ServiceProvider",
	description = "Abstract class that stores and provides singleton classes.",
	abstract = true,
	since = "0.1.0",

	signals = {
		Close = {
			description = "Fired when this service provider is closed.",
		},
		ServiceAdded = {
			description = "Fired when a service is added to this instance",
			args = { t.Instance },
		},
		ServiceRemoving = {
			description = "Fired when a service is about to be removed from to this instance",
			args = { t.Instance },
		},
	},

	methods = {
		FindService = {
			description = "Returns the named service if it exists, or otherwise nil.",
			params = { p("serviceName", t.String) },
			returns = t.Optional(t.Instance),
		},
		GetService = {
			description = "Returns the named service, constructing if it doesn't exist.",
			params = { p("serviceName", t.String) },
			returns = t.Instance,
		},
	},
})
