const types = require("./types")

-- NOTE: Permission defaults to None, set to Never to disable reads/writes
export type Permission = "None" | "Plugin" | "LocalUser" | "EngineScript" | "Engine" | "Debug" | "Never"

export type ClassProperty = {
	read type: types.Type,
	read description: string,
	read unmodified: string,
	read serializable: boolean?,

	read overrideRead: string?,
	read overrideWrite: string?,

	read readLevel: Permission?,
	read writeLevel: Permission?,
}

export type ClassSignal = {
	read description: string,
	read args: { types.Type }?,
}

export type ClassParameter = { read name: string, read type: types.Type }
export type ClassMethod = {
	read description: string,
	read level: Permission?,

	read params: { ClassParameter }?,
	read returns: types.Type?,

	--- Whether to output static int(lua_State* L, Instance* inst)
	read asLuau: boolean?,
	read luauSignature: string?,
}

export type Class = {
	read name: string,
	read description: string,
	read since: string,

	read root: boolean?,
	read abstract: boolean?,
	read extends: string?,
	read deprecated: boolean?,

	read properties: { [string]: ClassProperty }?,
	read signals: { [string]: ClassSignal }?,
	read methods: { [string]: ClassMethod }?,
}

const classes = {}

function classes.define(x: Class): Class
	return table.freeze(x)
end

function classes.parameter(name: string, type: types.Type): ClassParameter
	return table.freeze({ name = name, type = type })
end

function classes.unpack()
	return classes.define, types, classes.parameter
end

return table.freeze(classes)
