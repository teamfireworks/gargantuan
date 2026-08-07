const types = require("./types")

-- NOTE: ScriptLevel defaults to None, set to Never to disable reads/writes
export type ScriptLevel = "None" | "Never"

export type InstanceProperty = {
	read type: types.Type,
	read description: string,
	read unmodified: string,
	read serializable: boolean?,

	read overrideRead: string?,
	read overrideWrite: string?,

	read readLevel: ScriptLevel?,
	read writeLevel: ScriptLevel?,
}

export type InstanceSignal = {
	read description: string,
	read args: { types.Type }?,
}

export type InstanceParameter = { read name: string, read type: types.Type }
export type InstanceMethod = {
	read description: string,
	read params: { InstanceParameter }?,
	read returns: types.Type?,
	read level: ScriptLevel?,
}

export type InstanceClass = {
	read name: string,
	read description: string,
	read since: string,

	read abstract: boolean?,
	read extends: string?,
	read deprecated: boolean?,

	read properties: { [string]: InstanceProperty }?,
	read signals: { [string]: InstanceSignal }?,
	read methods: { [string]: InstanceMethod }?,
}

const inst = {
	types = types,
}

function inst.class(x: InstanceClass): InstanceClass
	return table.freeze(x)
end

function inst.type(x: types.Type): types.Type
	return x
end

function inst.p(name: string, type: types.Type): InstanceParameter
	return table.freeze({ name = name, type = type })
end

function inst.unpack()
	return inst, types, inst.p
end

return table.freeze(inst)
