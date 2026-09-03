-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const classes = require("./classes")
const orderedPairs = require("@gluten/utils/orderedPairs")
local formatSchemaIdentifier = require("./formatSchemaIdentifier")

const METAMETHOD_OVERRIDES: { [string]: string } = {
	__add = "operator+",
	__sub = "operator-",
	__div = "operator/",
	__mul = "operator*",
	__idiv = "operator//",
	__unm = "operator~",
	__eq = "operator==",
	__lt = "operator<",
	__call = "operator()",

	-- TODO: How tf do we codegen this so its pretty
	-- __??? = "operator<=>",
}

const function formatReadMethod(name: string, property: classes.ClassProperty, key: string)
	const implement = property.ImplementMethod or nil :: never

	local formatted = property.Type.CType
	formatted ..= " "
	formatted ..= key
	formatted ..= name
	formatted ..= "()"

	if not implement.CanMutate then
		formatted ..= " const"
	end

	if not implement.CanThrow then
		formatted ..= " noexcept"
	end

	formatted ..= "; \\"
	return formatted
end

const function formatWriteMethod(name: string, property: classes.ClassProperty, key: string)
	const implement = property.ImplementMethod or nil :: never

	local formatted = "void"
	formatted ..= " "
	formatted ..= key
	formatted ..= name
	formatted ..= "("

	formatted ..= property.Type.CType
	formatted ..= " value)"

	if not implement.CanThrow then
		formatted ..= " noexcept"
	end

	formatted ..= "; \\"
	return formatted
end

const function generateMacroBody(schema: classes.Schema<any, any>)
	const body: { string } = {}

	-- TODO: make this protected. the intent here is to fire like propchangedsignal
	-- for instances but we gotta get data types up first
	table.insert(body, "protected: \\")
	for name, member in orderedPairs(schema.Members) do
		if member.Kind == "ClassProperty" and (member.ImplementMember or member.ImplementPrivateMember) then
			table.insert(body, `{member.Type.CType} {name} = {member.UnmodifiedValue}; \\`)
		end
	end

	table.insert(body, "public: \\")
	for name, member in orderedPairs(schema.Members) do
		if member.Kind == "ClassProperty" then
			if member.ImplementManual then
				for key in member.Permits :: { [string]: classes.SecurityCapabilities } do
					local formatted = "static int "
					formatted ..= key
					formatted ..= name
					formatted ..= "(lua_State* L)"

					if member.ImplementManual.NoThrow then
						formatted ..= " noexcept"
					end

					formatted ..= "; \\"
					table.insert(body, formatted)
				end
			elseif member.ImplementMethod then
				if member.Permits.Read then
					table.insert(body, formatReadMethod(name, member, "Read"))
				end
				if member.Permits.ReadParallel then
					table.insert(body, formatReadMethod(name, member, "ReadParallel"))
				end
				if member.Permits.Write then
					table.insert(body, formatWriteMethod(name, member, "Read"))
				end
				if member.Permits.WriteParallel then
					table.insert(body, formatWriteMethod(name, member, "ReadParallel"))
				end
			end
		elseif member.Kind == "ClassConstant" then
			table.insert(body, `static {member.Type.CType} {name} = {member.Value}; \\`)
		elseif member.Kind == "ClassSignal" then
			-- signals arent supported for now
		else
			if member.ImplementManual then
				local formatted = "static int "
				formatted ..= name
				formatted ..= "(lua_State* L)"

				if member.ImplementManual.NoThrow then
					formatted ..= " noexcept"
				end

				formatted ..= "; \\"
				table.insert(body, formatted)
			elseif member.ImplementMethod then
				const isConstructor = name == "new"
				const isStatic = isConstructor or member.Kind == "ClassStaticMethod"
				const staticPrefix = not isConstructor and isStatic and "static " or ""

				local formatted = staticPrefix
				if isConstructor then
					formatted ..= formatSchemaIdentifier(schema)
				else
					formatted ..= member.ImplementMethod.ReturnType.CType
					formatted ..= " "
					formatted ..= METAMETHOD_OVERRIDES[name] or name
				end
				formatted ..= "("

				const numParameters = #member.ImplementMethod.Parameters
				for index, parameter in member.ImplementMethod.Parameters do
					if not isConstructor and not member.ImplementMethod.CanMutate then
						formatted ..= "const "
					end

					formatted ..= parameter.Type.CType
					formatted ..= " "

					-- TODO: Types need to be modellable so we can choose to
					-- omit references for primitive types like floats
					if not isConstructor and not member.ImplementMethod.CanMutate then
						formatted ..= " &"
					end

					formatted ..= parameter.Name

					if parameter.DefaultValue then
						formatted ..= " = "
						formatted ..= parameter.DefaultValue
					end

					if index ~= numParameters then
						formatted ..= ", "
					end
				end

				formatted ..= ")"

				if not isStatic and not member.ImplementMethod.CanMutate then
					formatted ..= " const"
				end

				if not member.ImplementMethod.CanThrow then
					formatted ..= " noexcept"
				end

				formatted ..= "; \\"
				table.insert(body, formatted)
			end
		end
	end

	return body
end

return generateMacroBody
