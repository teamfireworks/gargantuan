-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const tableext = require("@std/tableext")

const types = {}
types.__index = types

export type Type = setmetatable<{
	CType: string,
	CIncludes: { string }?,
	CForwardDeclaration: string?,
	LuauType: string,
}, typeof(types)>

const function collectTypes(len: number, ...: Type)
	const cTypes, luauTypes, cIncludes: { string } = table.create<<string>>(len), table.create<<string>>(len), {}
	local CForwardDeclaration: string? = nil

	for index = 1, select("#", ...) do
		const value: Type = select(index, ...)
		table.insert(cTypes, value.CType)
		table.insert(luauTypes, value.LuauType)

		if value.CIncludes then
			tableext.extend(cIncludes, value.CIncludes)
		end

		if value.CForwardDeclaration then
			if CForwardDeclaration == nil then
				CForwardDeclaration = ""
			end
			value.CForwardDeclaration ..= value.CForwardDeclaration .. "\n"
		end
	end

	return cTypes, cIncludes, CForwardDeclaration, luauTypes
end

const function newtype(cType: string, cIncludes: { string }?, CForwardDeclaration: string?, luauType: string): Type
	return setmetatable({
		CType = cType,
		CIncludes = cIncludes,
		CForwardDeclaration = CForwardDeclaration,
		LuauType = luauType,
	}, types)
end

const function datatype(name: string, cType: string?): Type
	return newtype(
		cType or `gg_{name}`,
		{
			-- `"datatypes/gg_{name}.hpp"`
		} :: { string }?,
		`struct gg_{name}` :: string?,
		`gg_{name}`
	)
end

types.Float = newtype("float", nil, nil, "number")
types.Double = newtype("double", nil, nil, "number")
types.Int8 = newtype("int8_t", { "<cstdint>" }, nil, "number")
types.Int16 = newtype("int16_t", { "<cstdint>" }, nil, "number")
types.Int32 = newtype("int32_t", { "<cstdint>" }, nil, "number")
types.Uint8 = newtype("uint8_t", { "<cstdint>" }, nil, "number")
types.Uint16 = newtype("uint16_t", { "<cstdint>" }, nil, "number")
types.Uint32 = newtype("uint32_t", { "<cstdint>" }, nil, "number")
types.Bool = newtype("bool", nil, nil, "boolean")
types.CString = newtype("const char *", nil, nil, "string")
types.String = newtype("std::string", { "<string>" }, nil, "string")
types.StringView = newtype("std::string_view", { "<string_view>" }, nil, "string")
types.Void = newtype("void", nil, nil, "()")

types.Axes = datatype("Axes")
types.Color3 = datatype("Color3")
types.CFrame = datatype("CFrame")
types.SecurityCapabilities = datatype("SecurityCapabilities")
types.Vector2 = datatype("Vector2")

types.Vector3 = datatype("Vector3", "glm::vec3")
table.insert(types.Vector3.CIncludes or nil :: never, "<glm/glm.hpp>")

function types.Extend(
	self: Type,
	cType: string?,
	cIncludes: { string }?,
	CForwardDeclaration: string?,
	luauType: string?
): Type
	return setmetatable({
		CType = cType or self.CType,
		CIncludes = if cIncludes and self.CIncludes
			then tableext.extend(cIncludes, self.CIncludes)
			else self.CIncludes or cIncludes or {} :: { string }?,
		CForwardDeclaration = CForwardDeclaration or self.CForwardDeclaration,
		LuauType = luauType or self.LuauType,
	}, types)
end

function types.Vector(inner: Type): Type
	return inner:Extend(`std::vector<{inner.CType}>`, { "<vector>" }, nil, `\{ {inner.CType} }`)
end

function types.Optional(inner: Type): Type
	return inner:Extend(`std::optional<{inner.CType}>`, { "<optional>" }, nil, `{inner.CType}?`)
end

function types.Tuple(...: Type): Type
	const len = select("#", ...)
	if len == 0 then
		return types.Void
	elseif len == 1 then
		return ... :: Type
	end

	const cTypes, cIncludes, CForwardDeclaration, luauTypes = collectTypes(len, ...)
	table.insert(cIncludes, "<tuple>")
	return newtype(
		`std::tuple<{table.concat(cTypes, ", ")}>`,
		cIncludes,
		CForwardDeclaration,
		`({table.concat(luauTypes, ", ")})`
	)
end

return table.freeze(types)
