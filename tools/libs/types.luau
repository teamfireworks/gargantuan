export type Type = {
	read cType: string,
	read cIncludes: { string }?,
	read luauType: string,

	read optionalValue: Type?,
}

const function type(x: Type): Type
	return x
end

const function datatype(typename: string, cType: string?): Type
	return table.freeze({
		cType = cType or "gargantuan::" .. typename,
		cIncludes = { `"gargantuan/datatypes/{typename}.hpp"` },
		luauType = typename,
		optionalValue = nil,
	})
end

const types = {
	Bool = type({ cType = "bool", luauType = "boolean" }),
	Double = type({ cType = "double", luauType = "number" }),
	Float = type({ cType = "float", luauType = "number" }),
	Integer = type({ cType = "int", luauType = "number" }),
	String = type({ cType = "std::string", cIncludes = { "<string>" }, luauType = "string" }),
	StringPointer = type({ cType = "const char *", luauType = "string" }),
	StringView = type({ cType = "std::string_view", cIncludes = { "<string_view>" }, luauType = "string" }),

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
		cType = "std::shared_ptr<gargantuan::Instance>",
		-- It's very likely the user has already introduced Instance.hpp
		-- somewhere, and including it here leads to a recursive import with
		-- the main Instance class. We omit the Instance include here.
		cIncludes = { "<memory>" },
		luauType = "Instance",
	}),

	Signal = type({
		cType = "std::shared_ptr<gargantuan::BaseSignal>",
		cIncludes = { "<memory>", '"gargantuan/datatypes/Signal.hpp"' },
		luauType = "Signal<...any>",
	}),

	Vector3 = type({ cType = "glm::vec3", cIncludes = { "<glm/glm.hpp>" }, luauType = "Vector3" }),
}

function types.IsA(className: string): Type
	return {
		cType = `gargantuan::{className}`,
		cIncludes = { `"gargantuan/classes/{className}.hpp"` },
		luauType = className,
	}
end

function types.EnumItem(enumName: string?): Type
	if not enumName then
		return {
			cType = "gargantuan::EnumItem",
			cIncludes = { `"gargantuan/reflection/Enums.hpp"` },
			luauType = "EnumItem",
		}
	else
		return {
			cType = `gargantuan::Enums::{enumName}`,
			cIncludes = { `"gargantuan/reflection/Enums.hpp"` },
			luauType = `Enum.{enumName}`,
		}
	end
end

function types.Optional(base: Type): Type
	local cIncludes = base.cIncludes or {} :: never
	table.insert(cIncludes, "<optional>")

	return {
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
		cType = `std::vector<{base.cType}>`,
		cIncludes = cIncludes,
		luauType = `\{ {base.luauType} \}`,
	}
end

return table.freeze(types)
