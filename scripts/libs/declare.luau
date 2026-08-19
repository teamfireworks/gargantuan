const typesLib = require("@tools/types")

export type ThreadLevel = "None" | "Plugin" | "LocalUser" | "EngineScript" | "Engine" | "Debug" | "Never"

export type Parameter = {
	read name: string,
	read type: typesLib.Type,

	[string]: never,
}

export type Overload = {
	read name: string,
	read description: string,
	read returns: typesLib.Type,
	read threadLevel: ThreadLevel?,

	[string]: never,
}

export type Property = {
	read description: string,
	read since: string?,

	read type: typesLib.Type,
	read unmodified: string,

	read readLevel: ThreadLevel?,
	read writeLevel: ThreadLevel?,

	--- Automatically generate a class member alongside default get/setters
	read withMember: { [string]: never }?,

	--- Manually define get and set methods
	read withManual: { read constant: boolean?, read getMethod: string, read setMethod: string, [string]: never }?,

	[string]: never,
}

export type Signal = {
	read description: string,
	read since: string?,

	read parameters: { Parameter },
	read restParameter: { type: typesLib.Type, description: string }?,
	read threadLevel: ThreadLevel?,

	[string]: never,
}

export type Method = {
	read description: string,
	read since: string?,

	--- Automatically generate a matching Luau implementation for this method
	read withMember: {
		read threadLevel: ThreadLevel?,
		--- Whether to mark as a `const` method.
		read constant: boolean?,
		read parameters: { Parameter }?,
		read returns: typesLib.Type?,
		[string]: never,
	}?,

	--- Automatically generate a matching Luau implementation using overloads
	read withOverloads: { [typesLib.Type | { typesLib.Type }]: Overload }?,

	--- Manually define a static int(lua_State*)
	read withManual: { read luauSignature: string, [string]: never }?,

	[string]: never,
}

export type StaticProperty = {
	read description: string,
	read since: string?,

	read type: typesLib.Type,
	read value: string,

	[string]: never,
}

type function WithKind(kind: type, target: type)
	local result = types.copy(target)
	result:setproperty(types.singleton("kind"), types.copy(kind))
	return result
end

export type Member = WithKind<"property", Property> | WithKind<"signal", Signal> | WithKind<"method", Method>
export type LibraryMember = WithKind<"static", StaticProperty> | WithKind<"signal", Signal> | WithKind<"method", Method>

export type Class = {
	read name: string,
	read robloxName: string?,
	read extends: string?,
	read abstract: boolean?,
	read description: string,
	read since: string,

	read members: { [string]: Member }?,

	[string]: never,
}

export type Datatype = {
	read name: string,
	read luauType: string?,
	read robloxName: string?,
	read robloxType: string?,
	read description: string,
	read since: string,

	read members: { [string]: Member }?,
	read library: { [string]: Member }?,

	[string]: never,
}

const function kindConstructor<T, K>(kind: K | ""): (x: T) -> WithKind<K, T>
	return function(x: T)
		const result = table.clone(x) :: WithKind<K, T>
		(result :: any).kind = kind
		return result
	end
end

const declare = {
	types = typesLib,

	property = kindConstructor<<Property>>("property"),
	static = kindConstructor<<StaticProperty>>("static"),
	signal = kindConstructor<<Signal>>("signal"),
	method = kindConstructor<<Method>>("method"),

	class = kindConstructor<<Class>>("class"),
	datatype = kindConstructor<<Datatype>>("datatype"),
}

function declare.parameter(name: string, type: typesLib.Type): Parameter
	return { name = name, type = type }
end

function declare.unpack()
	return declare, declare.types, declare.parameter
end

return table.freeze(declare)
