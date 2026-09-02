local prefixLines = require("@gluten/prefixLines")
local tableext = require("@std/tableext")
local useConfiguration = require("@gluten/useConfiguration")

const function generateMacroHeader(configuration: useConfiguration.Configuration?)
	configuration = configuration or useConfiguration()

	const body = {}
	tableext.extend(body, prefixLines("//", configuration.gargantuan.licenseHeader))
	table.insert(body, "")
	tableext.extend(body, prefixLines("//", configuration.gargantuan.codegenHeader))
	table.insert(body, "")
	table.insert(body, "#pragma once")
	return body
end

return generateMacroHeader
