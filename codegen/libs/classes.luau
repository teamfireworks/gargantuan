-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const theTypes = require("@codegen/libs/types")
type Type = theTypes.Type

export type SecurityCapability =
	| "AccessOutsideWrite"
	| "Animation"
	| "AssetCreateUpdate"
	| "AssetManagement"
	| "AssetRead"
	| "AssetRequire"
	| "Audio"
	| "Avatar"
	| "Basic"
	| "CapabilityControl"
	| "Capture"
	| "Chat"
	| "Consequences"
	| "CreateInstances"
	| "CSG"
	| "DataStore"
	| "DynamicGeneration"
	| "Environment"
	| "Groups"
	| "Input"
	| "InternalTest"
	| "LegacySound"
	| "LoadString"
	| "Logging"
	| "Material"
	| "Network"
	| "Physics"
	| "Players"
	| "Plugin"
	| "PluginOrOpenCloud"
	| "PromptExternalPurchase"
	| "RemoteEvent"
	| "RunClientScript"
	| "RunServerScript"
	| "ScriptGlobals"
	| "SensitiveInput"
	| "ServerCommunication"
	| "Teleport"
	| "UI"
	| "Unassigned"

export type SecurityCapabilities = { SecurityCapability }

export type FunctionParameter = {
	Name: string,
	Description: string,
	Type: Type,
	DefaultValue: string?,
	[string]: never,
}

export type FunctionOverload = {
	Description: string,
	Since: vector?,
	CanMutate: boolean?,
	CanThrow: boolean?,
	CanYield: boolean?,
	ReturnType: Type,
	Permits: {
		Call: SecurityCapabilities?,
		CallParallel: SecurityCapabilities?,
	}?,
}

export type ClassProperty = {
	--- Only relevant in InstanceClasses
	Serialized: boolean?,
	Type: Type,
	UnmodifiedValue: string,
	Display: {
		Description: string,
		DeprecationMessage: string?,
		Since: vector?,
		--- Only relevant in InstanceClasses
		Category: string?,
	},
	Permits: {
		Read: SecurityCapabilities?,
		ReadParallel: SecurityCapabilities?,
		Write: SecurityCapabilities?,
		WriteParallel: SecurityCapabilities?,
	},
	--- Output a class member with default reads and writes
	ImplementMember: boolean?,
	--- Output header methods to implement reads and writes
	ImplementMethod: {
		CanMutate: boolean?,
		CanThrow: boolean?,
	}?,
	--- Output static int(lua_State *)
	ImplementManual: {
		NoThrow: boolean?,
	}?,
}

export type ClassMethod = {
	Display: {
		Description: string,
		DeprecationMessage: string?,
		Since: vector?,
	},
	--- Output a header method
	ImplementMethod: {
		CanMutate: boolean?,
		CanThrow: boolean?,
		CanYield: boolean?,

		Parameters: { FunctionParameter },
		ReturnType: Type,

		Permits: {
			Call: SecurityCapabilities?,
			CallParallel: SecurityCapabilities?,
		},
	}?,
	--- Output header methods alongside a default Luau implementation
	ImplementOverloaded: {
		[{ FunctionParameter }]: FunctionOverload,
	}?,
	--- Output static int(lua_State *)
	ImplementManual: {
		NoThrow: boolean?,
		CanYield: boolean?,

		Permits: {
			Call: SecurityCapabilities?,
			CallParallel: SecurityCapabilities?,
		},
	}?,
}

export type ClassStaticMethod = ClassMethod

export type ClassSignal = {
	Display: {
		Description: string,
		Since: vector?,
	},
	Parameters: { FunctionParameter },
	Permits: {
		Listen: SecurityCapabilities?,
	},
}

export type ClassConstant = {
	Display: {
		Description: string,
		Since: vector,
	},
	Type: Type,
	Value: string,
}

export type function WithKind(target: type, kind: type)
	const result = types.copy(target)
	result:setproperty(types.singleton("Kind"), kind)
	return result
end

export type ClassMember =
	| WithKind<ClassProperty, "ClassProperty">
	| WithKind<ClassMethod, "ClassMethod">
	| WithKind<ClassStaticMethod, "ClassStaticMethod">
	| WithKind<ClassSignal, "ClassSignal">
	| WithKind<ClassConstant, "ClassConstant">

export type Members = { [string]: ClassMember }
export type Schema<T, K> = {
	Kind: K,
	Prelude: T,
	Members: Members,
}

export type InstanceClass = {
	Name: string,
	Owner: string?,
	Abstract: boolean?,
	Serializable: boolean?,
	Description: string,
	Since: vector,
}

export type InstanceClassSchema = Schema<InstanceClass, "InstanceClass">

export type Datatype = {
	Name: string,
	Description: string,
	Since: vector,
}

export type DatatypeSchema = Schema<Datatype, "Datatype">

const function createKindedConstructor<T, K>(kind: K | "")
	return function(x: T): WithKind<T, K>
		(x :: any).Kind = kind
		return x
	end
end

const classes = {
	Property = createKindedConstructor<<ClassProperty>>("ClassProperty"),
	Method = createKindedConstructor<<ClassMethod>>("ClassMethod"),
	StaticMethod = createKindedConstructor<<ClassStaticMethod>>("ClassStaticMethod"),
	Signal = createKindedConstructor<<ClassSignal>>("ClassSignal"),
	Constant = createKindedConstructor<<ClassConstant>>("ClassConstant"),

	Parameter = function(name: string, description: string, type: Type, defaultValue: string?): FunctionParameter
		return {
			Name = name,
			Description = description,
			Type = type,
			DefaultValue = defaultValue,
		}
	end,
}

return table.freeze(classes)
