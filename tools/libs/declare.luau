const ctypes = require("@tools/ctypes")

export type ThreadLevel = "None" | "Plugin" | "LocalUser" | "CoreScript" | "Core" | "Debug" | "Never"

export type Parameter = {
	read name: string,
	read type: ctypes.CType,

	[string]: never,
}

export type Overload = {
	read name: string,
	read description: string,
	read returns: { ctypes.CType }?,
	read threadLevel: ThreadLevel?,

	[string]: never,
}

export type Property = {
	read robloxName: string?,
	read description: string,
	read since: string?,

	read type: ctypes.CType,
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
	read robloxName: string?,
	read description: string,
	read since: string?,

	read parameters: { Parameter },
	read restParameter: { type: ctypes.CType, description: string }?,
	read threadLevel: ThreadLevel?,

	[string]: never,
}

export type Method = {
	read robloxName: string?,
	read description: string,
	read since: string?,

	--- Automatically generate a matching Luau implementation for this method
	read withMember: {
		read threadLevel: ThreadLevel?,
		--- Whether to mark as a `const` method.
		read constant: boolean?,
		read parameters: { Parameter }?,
		read returns: { ctypes.CType }?,
		[string]: never,
	}?,

	--- Automatically generate a matching Luau implementation using overloads
	read withOverloads: { [ctypes.CType | { ctypes.CType }]: Overload }?,

	--- Manually define a static int(lua_State*)
	read withManual: { read luauSignature: string, [string]: never }?,

	[string]: never,
}

export type StaticProperty = {
	read robloxName: string?,
	read description: string,
	read since: string?,

	read type: ctypes.CType,
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
	types = ctypes,

	property = kindConstructor<<Property>>("property"),
	static = kindConstructor<<StaticProperty>>("static"),
	signal = kindConstructor<<Signal>>("signal"),
	method = kindConstructor<<Method>>("method"),

	class = kindConstructor<<Class>>("class"),
	datatype = kindConstructor<<Datatype>>("datatype"),
}

function declare.parameter(name: string, type: ctypes.CType): Parameter
	return { name = name, type = type }
end

function declare.unpack()
	return declare, declare.types, declare.parameter
end

return table.freeze(declare)
