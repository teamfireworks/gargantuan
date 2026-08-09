export type Type = {
	read kind: "c" | "class" | "datatype" | "enum" | "optional",
	read cType: string,
	read cIncludes: { string }?,
	read cForwardStatement: string?,
	read luauType: string,
}

const function type(x: Type): Type
	return x
end

const function literal<T>(x: T | ""): T
	return x
end

const function datatype(typename: string, cType: string?): Type
	return table.freeze({
		kind = literal("datatype"),
		cType = cType or "gargantuan::" .. typename,
		cIncludes = { `"gargantuan/datatypes/{typename}.hpp"` },
		cForwardStatement = nil,
		luauType = typename,
	})
end

const types = {
	Bool = type({ kind = literal("c"), cType = "bool", luauType = "boolean" }),
	Double = type({ kind = literal("c"), cType = "double", luauType = "number" }),
	Float = type({ kind = literal("c"), cType = "float", luauType = "number" }),
	Integer = type({ kind = literal("c"), cType = "int", luauType = "number" }),
	String = type({ kind = literal("c"), cType = "std::string", cIncludes = { "<string>" }, luauType = "string" }),
	StringPointer = type({ kind = literal("c"), cType = "const char *", luauType = "string" }),
	StringView = type({
		kind = literal("c"),
		cType = "std::string_view",
		cIncludes = { "<string_view>" },
		luauType = "string",
	}),

	Axes = datatype("Axes"),
	CFrame = datatype("CFrame"),
	Color3 = datatype("Color3"),
	Font = datatype("Font"),
	Random = datatype("Random"),
	Rect = datatype("Rect"),
	TweenInfo = datatype("TweenInfo"),
	UDim = datatype("UDim"),
	UDim2 = datatype("UDim2"),
	Vector2 = datatype("Vector2"),

	Instance = type({
		kind = "class",
		cType = "std::shared_ptr<gargantuan::Instance>",
		cIncludes = { "<memory>", '"gargantuan/classes/Instance.hpp"' },
		cForwardStatement = `class Instance;`,
		luauType = "Instance",
	}),

	Signal = type({
		kind = "datatype",
		cType = "std::shared_ptr<gargantuan::BaseSignal>",
		cIncludes = { "<memory>", '"gargantuan/datatypes/Signal.hpp"' },
		luauType = "Signal<...any>",
	}),

	Vector3 = type({
		kind = "datatype",
		cType = "glm::vec3",
		cIncludes = { '"gargantuan/datatypes/Vector3.hpp"' },
		luauType = "Vector3",
	}),
}

function types.IsA(className: string): Type
	return {
		kind = "class",
		cType = `std::shared_ptr<gargantuan::{className}>`,
		cIncludes = { `"gargantuan/classes/{className}.hpp"` },
		cForwardStatement = `class {className};`,
		luauType = className,
	}
end

function types.EnumItem(enumName: string?): Type
	if not enumName then
		return {
			kind = "enum",
			cType = "gargantuan::EnumItem",
			cIncludes = { `"gargantuan/reflection/Enums.hpp"` },
			cForwardStatement = `class EnumItem;`,
			luauType = "EnumItem",
		}
	else
		return {
			kind = "enum",
			cType = `gargantuan::Enums::{enumName}`,
			cIncludes = { `"gargantuan/reflection/Enums.hpp"` },
			cForwardStatement = `namespace Enums \{ enum {enumName}; };`,
			luauType = `Enum.{enumName}`,
		}
	end
end

function types.Optional(base: Type): Type
	local cIncludes = base.cIncludes or {} :: never
	table.insert(cIncludes, "<optional>")

	return {
		kind = "optional",
		cType = `std::optional<{base.cType}>`,
		cIncludes = cIncludes,
		luauType = `{base.luauType}?`,
		optionalValue = base,
	}
end

function types.Array(base: Type): Type
	local cIncludes = base.cIncludes or {} :: never
	table.insert(cIncludes, "<vector>")

	return {
		kind = "c",
		cType = `std::vector<{base.cType}>`,
		cIncludes = cIncludes,
		luauType = `\{ {base.luauType} }`,
	}
end

return table.freeze(types)
