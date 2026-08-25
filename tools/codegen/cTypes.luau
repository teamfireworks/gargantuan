const ctypes = {}
ctypes.__index = ctypes

type Self = { cType: string, cIncludes: { string }?, cForwardStatement: string?, luauType: string }
export type CType = setmetatable<Self, typeof(ctypes)>

const function datatype(name: string, cType: string?)
	return ctypes.new({
		cType = `gg_{name}`,
		cIncludes = { `datatypes/gg_{name}` },
		cForwardStatement = `class gg_{name}`,
		luauType = name,
	})
end

function ctypes.new(inner: Self): CType
	return setmetatable(inner, ctypes)
end

function ctypes.Clone(self: CType)
	return ctypes.new({
		cType = self.cType,
		cIncludes = self.cIncludes and table.clone(self.cIncludes),
		cForwardStatement = self.cForwardStatement,
		luauType = self.luauType,
	})
end

function ctypes.__tostring(self: CType)
	return self.cType
end

ctypes.Int32 = ctypes.new({ cType = "int32_t", cIncludes = { "<cstdint>" }, luauType = "number" })
ctypes.Int16 = ctypes.new({ cType = "int16_t", cIncludes = { "<cstdint>" }, luauType = "number" })
ctypes.Int8 = ctypes.new({ cType = "int8_t", cIncludes = { "<cstdint>" }, luauType = "number" })
ctypes.UInt32 = ctypes.new({ cType = "uint32_t", cIncludes = { "<cstdint>" }, luauType = "number" })
ctypes.UInt16 = ctypes.new({ cType = "uint16_t", cIncludes = { "<cstdint>" }, luauType = "number" })
ctypes.UInt8 = ctypes.new({ cType = "uint8_t", cIncludes = { "<cstdint>" }, luauType = "number" })
ctypes.Float = ctypes.new({ cType = "float", luauType = "number" })
ctypes.Double = ctypes.new({ cType = "double", luauType = "number" })
ctypes.Bool = ctypes.new({ cType = "bool", luauType = "boolean" })
ctypes.CString = ctypes.new({ cType = "const char *", luauType = "string" })
ctypes.String = ctypes.new({ cType = "std::string", cIncludes = { "<string>" }, luauType = "string" })
ctypes.StringView = ctypes.new({ cType = "std::string_view", cIncludes = { "<string_view>" }, luauType = "string" })

ctypes.Color3 = datatype("Color3")

function ctypes.Instance(className: string?): CType
	className = className or "Instance"

	return ctypes.new({
		cType = `std::shared_ptr<gg_{className}>`,
		cIncludes = { `"datamodel/gg_{className}.hpp"` },
		cForwardStatement = `class gg_{className};`,
		luauType = className,
	})
end

function ctypes.EnumItem(enumName: string?): CType
	if not enumName then
		return ctypes.new({
			cType = "gg_EnumItem",
			cIncludes = { `"core/reflection/EnumRegistry.hpp"` },
			cForwardStatement = `class gg_EnumItem;`,
			luauType = "EnumItem",
		})
	else
		return ctypes.new({
			cType = `gg_{enumName}`,
			cIncludes = { `"core/reflection/EnumRegistry.hpp"` },
			cForwardStatement = `enum gg_{enumName};`,
			luauType = `Enum.{enumName}`,
		})
	end
end

function ctypes.Signal(...: CType): CType
	const numArgs = select("#", ...)
	if numArgs == 0 then
		return ctypes.new({
			cType = "gg_Signal<std::monostate>",
			cIncludes = { "<variant>", '"datatypes/gg_Signal.hpp"' },
			luauType = "Signal<>",
		})
	elseif numArgs == 1 then
		const type: CType = ...
		const cIncludes = { "<memory>", '"datatypes/gg_Signal.hpp"' }

		if type.cIncludes then
			table.move(type.cIncludes, 1, #type.cIncludes, #cIncludes + 1, cIncludes)
		end

		return ctypes.new({
			cType = `gg_Signal<{type.cType}>`,
			cIncludes = { '"datatypes/gg_Signal.hpp"' },
			luauType = `Signal<{type.luauType}>`,
		})
	else
		const cIncludes = { "<tuple>", '"datatypes/gg_Signal.hpp"' }
		const cArgs, luauArgs = table.create(numArgs), table.create(numArgs)

		for index = 1, numArgs do
			const type: CType = select(index, ...)
			if type.cIncludes then
				table.move(type.cIncludes, 1, #type.cIncludes, #cIncludes + 1, cIncludes)
			end
			cArgs[index] = type.cType
			luauArgs[index] = type.luauType
		end

		return ctypes.new({
			cType = `gg_Signal<std::tuple<{table.concat(cArgs, ", ")}>>`,
			cIncludes = cIncludes,
			luauType = `Signal<{table.concat(luauArgs, ", ")}>`,
		})
	end
end

function ctypes.Optional(base: CType): CType
	const cIncludes = base.cIncludes or {} :: never
	table.insert(cIncludes, "<optional>")

	return ctypes.new({
		cType = `std::optional<{base.cType}>`,
		cIncludes = cIncludes,
		cForwardStatement = base.cForwardStatement,
		luauType = base.luauType,
	})
end

function ctypes.Array(base: CType): CType
	local cIncludes = base.cIncludes or {} :: never
	table.insert(cIncludes, "<vector>")

	return ctypes.new({
		cType = `std::vector<{base.cType}>`,
		cIncludes = cIncludes,
		cForwardStatement = base.cForwardStatement,
		luauType = `\{ {base.luauType} }`,
	})
end

function ctypes.Function(...: { name: string, type: CType })
	const cIncludes = { "<functional>" }

	const numParams = select("#", ...)
	local cParams, luauParams = "", ""
	if numParams ~= 0 then
		for index = 1, numParams do
			const param: { name: string, type: CType } = select(index, ...)
			cParams ..= param.type.cType .. " " .. param.name
			luauParams ..= param.name .. ": " .. param.type.luauType

			if param.type.cIncludes then
				table.move(param.type.cIncludes, 1, #param.type.cIncludes, #cIncludes + 1, cIncludes)
			end

			if index ~= numParams then
				cParams ..= ", "
				luauParams ..= ", "
			end
		end
	end

	return function(...: CType): CType
		const numReturns = select("#")
		local cReturns: string, luauReturns: string

		if numReturns == 0 then
			cReturns, luauReturns = "void", "()"
		elseif numReturns == 1 then
			const ret = select(1, ...)
			cReturns, luauReturns = ret.cType, ret.luauType
			if ret.cIncludes then
				table.move(ret.cIncludes, 1, #ret.cIncludes, #cIncludes + 1, cIncludes)
			end
		else
			table.insert(cIncludes, "<tuple>")
			cReturns, luauReturns = "std::tuple<", "("
			for index = 1, numReturns do
				const ret: CType = select(index, ...)
				cReturns ..= ret.cType
				luauReturns ..= ret.luauType

				if ret.cIncludes then
					table.move(ret.cIncludes, 1, #ret.cIncludes, #cIncludes + 1, cIncludes)
				end

				if index ~= numReturns then
					cReturns ..= ", "
					luauReturns ..= ", "
				end
			end
		end

		return ctypes.new({
			cType = `std::function<{cReturns}({cParams})>`,
			cIncludes = cIncludes,
			luauType = `({luauParams}) -> {luauReturns}`,
		})
	end
end

function ctypes.Shared(base: CType): CType
	const cIncludes = base.cIncludes or {} :: never
	table.insert(cIncludes, "<optional>")

	return ctypes.new({
		cType = `std::shared_ptr<{base.cType}>`,
		cIncludes = cIncludes,
		cForwardStatement = base.cForwardStatement,
		luauType = base.luauType,
	})
end

function ctypes.Unique(base: CType): CType
	const cIncludes = base.cIncludes or {} :: never
	table.insert(cIncludes, "<optional>")

	return ctypes.new({
		cType = `std::unique_ptr<{base.cType}>`,
		cIncludes = cIncludes,
		cForwardStatement = base.cForwardStatement,
		luauType = base.luauType,
	})
end

function ctypes.Const(base: CType): CType
	return ctypes.new({
		cType = `const {base.cType}`,
		cIncludes = base.cIncludes,
		cForwardStatement = base.cForwardStatement,
		luauType = base.luauType,
	})
end

function ctypes.ConstRef(base: CType): CType
	return ctypes.new({
		cType = `const {base.cType} &`,
		cIncludes = base.cIncludes,
		cForwardStatement = base.cForwardStatement,
		luauType = base.luauType,
	})
end

function ctypes.Ref(base: CType): CType
	return ctypes.new({
		cType = `{base.cType} &`,
		cIncludes = base.cIncludes,
		cForwardStatement = base.cForwardStatement,
		luauType = base.luauType,
	})
end

return table.freeze(ctypes)
