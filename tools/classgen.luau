-- TODO: This file has like 20 hacks, we should probably implement the types
-- better. Utrain has a goated suggestions on how to better represent the types,
-- check Discord #development

const fs = require("@std/fs")
const path = require("@std/path")
const process = require("@std/process")
const luau = require("@std/luau")
local tableext = require("@std/tableext")
local classes = require("@tools/classes")
local types = require("@tools/types")

const TARGETS = {
	{
		input = path.join(process.cwd(), "assets/classes"),
		output = path.join(process.cwd(), "include/gargantuan/classes/generated"),
	},
	{
		input = path.join(process.cwd(), "assets/services"),
		output = path.join(process.cwd(), "include/gargantuan/services/generated"),
	},
}

const function orderedPairs<K, V>(tbl: { [K]: V }): () -> (K, V)
	const keys: { K } = {}
	local numKeys = 0
	for key in tbl do
		table.insert(keys, key)
		numKeys += 1
	end
	table.sort(keys)

	local currentIndex = 0
	return function()
		currentIndex += 1
		if currentIndex > numKeys then
			return nil, nil
		end

		const key = keys[currentIndex]
		return key, tbl[key]
	end
end

const function pushIncludes(type: types.Type, includes: { [string]: true })
	if type.cIncludes then
		for _, include in type.cIncludes do
			includes[include] = true
		end
	end
end

const function makeSuperclass(class: classes.Class, includes: { [string]: true })
	local protectedBody = { "\t  protected:" }
	local publicBody = { "\t  public:" }

	if class.properties then
		for key, property in orderedPairs(class.properties) do
			pushIncludes(property.type, includes)
			if property.overrideRead or property.overrideWrite then
				if property.overrideRead then
					table.insert(publicBody, `\t\t{property.type.cType} {property.overrideRead}();`)
				end
				if property.overrideWrite then
					table.insert(publicBody, `\t\tvoid {property.overrideWrite}({property.type.cType} value);`)
				end
			else
				table.insert(protectedBody, `\t\t{property.type.cType} {key} = {property.unmodified};`)
				table.insert(publicBody, `\t\t{property.type.cType} Get{key}();`)
				table.insert(publicBody, `\t\tvoid Set{key}({property.type.cType} value);`)
			end
		end
	end

	if class.signals then
		for key, signal in orderedPairs(class.signals) do
			local signalType: string
			if signal.args and #signal.args == 1 then
				signalType = signal.args[1].cType
			elseif signal.args and #signal.args > 1 then
				includes["<tuple>"] = true

				signalType = "std::tuple<"
				for index, type in signal.args do
					signalType ..= type.cType
					pushIncludes(type, includes)
					if index ~= #signal.args then
						signalType ..= ", "
					end
				end
				signalType ..= ">"
			else
				includes["<variant>"] = true
				signalType = "std::monostate"
			end

			includes['"gargantuan/datatypes/Signal.hpp"'] = true
			table.insert(publicBody, `\t\ttypedef Signal<{signalType}> {key}Type;`)
			table.insert(publicBody, `\t\tstd::shared_ptr<{key}Type> {key} = std::make_shared<{key}Type>();`)
		end
	end

	if class.methods then
		for key, method in orderedPairs(class.methods) do
			if method.asLuau then
				includes["<lua.h>"] = true
				table.insert(publicBody, `\t\tstatic int {key}(lua_State* L, Instance* inst);`)
			else
				const parameters = {}
				if method.params then
					for i, parameter in method.params do
						pushIncludes(parameter.type, includes)
						parameters[i] = `{parameter.type.cType} {parameter.name}`
					end
				end
				const returnType = method.returns and method.returns.cType or "void"
				table.insert(publicBody, `\t\t{returnType} {key}({table.concat(parameters, ", ")});`)
			end
		end
	end

	const baseClass = class.extends or "Instance"
	includes[`"gargantuan/classes/{baseClass}.hpp"`] = true

	const body = { "namespace gargantuan::BaseInstances {" }
	if class.name == "Instance" then
		table.insert(body, `\tclass {class.name} \{`)
	else
		table.insert(body, `\tclass {class.name} : public gargantuan::{baseClass} \{`)
	end

	tableext.extend(body, {
		"\t  public:",
		"\t\tstatic const InstanceClassDefinition CLASS_DEFINITION;",
	}, protectedBody, publicBody)
	table.insert(body, "\t};")
	table.insert(body, "};")
	return body
end

const function makeClassDeclarationMacro(class: classes.Class)
	return {
		`{class.name} : public BaseInstances::{class.name}`,
	}
end

const function makeClassImplementationBody(class: classes.Class, includes: { [string]: true })
	-- InstanceClassDefinition
	const body = {
		`const InstanceClassDefinition {class.name}::CLASS_DEFINITION \{`,
		`\t.ClassName = {string.format("%q", class.name)},`,
	}

	if not class.abstract then
		includes["<memory>"] = true
		table.insert(
			body,
			`\t.Constructor = []() -> std::shared_ptr<gargantuan::Instance> \{ return std::make_shared<gargantuan::{class.name}>(); },`
		)
	end

	if class.extends then
		table.insert(body, `\t.Superclass = {string.format("%q", class.extends)},`)
	elseif class.name == "Instance" then
		includes["<optional>"] = true
		table.insert(body, `\t.Superclass = std::nullopt,`)
	end

	table.insert(body, "\t.Properties = {")
	if class.properties then
		includes['"gargantuan/InstanceProperty.hpp"'] = true
		for key, property in orderedPairs(class.properties) do
			table.insert(body, `\t\tInstanceProperty({string.format("%q", key)})`)
			table.insert(body, `\t\t\t.SetReflectedTypedef({string.format("%q", property.type.luauType)})`)
			table.insert(body, `\t\t\t.SetUnmodified({property.unmodified})`)
			if property.serializable then
				table.insert(body, `\t\t\t.SetSerializable()`)
			end
			if property.overrideRead or not property.overrideWrite then
				table.insert(body, `\t\t\t.UseRead<{`&{class.name}::{property.overrideRead or `Get{key}`}`}>()`)
			end
			if property.overrideWrite or not property.overrideRead then
				table.insert(body, `\t\t\t.UseWrite<{`&{class.name}::{property.overrideWrite or `Set{key}`}`}>()`)
			end
			table.insert(body, `\t\t\t.IntoPair(),`)
		end
	end
	if class.signals then
		includes['"gargantuan/InstanceProperty.hpp"'] = true
		for key, signal in orderedPairs(class.signals) do
			const signalArgs = table.create<<string>>(signal.args and #signal.args or 0)
			if signal.args then
				for i, arg in signal.args do
					signalArgs[i] = arg.luauType
				end
			end

			table.insert(body, `\t\tInstanceProperty({string.format("%q", key)})`)
			table.insert(body, `\t\t\t.SetSignal()`)
			table.insert(body, `\t\t\t.SetReflectedTypedef("Signal<{table.concat(signalArgs, ", ")}>")`)
			table.insert(body, `\t\t\t.UseRead<&{class.name}::{key}>()`)
			table.insert(body, `\t\t\t.IntoPair(),`)
		end
	end
	table.insert(body, `\t},`)

	table.insert(body, `\t.Methods = \{`)
	if class.methods then
		for key, method in orderedPairs(class.methods) do
			if method.asLuau then
				table.insert(
					body,
					`\t\t\{"{key}", Method\{.Call = &{class.name}::{key}, .Signature = {string.format(
						"%q",
						method.luauSignature or ""
					)}}},`
				)
			else
				table.insert(body, `\t\t\{"{key}", Method::fromMember<&{class.name}::{key}>()},`)
			end
		end
	end
	table.insert(body, `\t},`)
	table.insert(body, "};")

	-- InstanceClassRegistry
	includes['"gargantuan/reflection/InstanceClassRegistry.hpp"'] = true
	table.insert(body, "namespace InstanceClassRegistry::__IGNORE__ {")
	table.insert(body, `\tbool {class.name} = []() \{`)
	table.insert(body, `\t\tInstanceClassRegistry::Register<gargantuan::{class.name}>({class.name}::CLASS_DEFINITION);`)
	table.insert(body, `\t\treturn true;`)
	table.insert(body, `\t}();`)
	table.insert(body, "};")

	-- Default implementations

	return body
end

const function renderMacroBody(macroName: string, body: { string })
	local rendered = `#define {macroName} {next(body) and "\\" or ""}\n`
	for index, line in body do
		rendered ..= `\t{line} {index == #body and "" or "\\"}\n`
	end
	return rendered
end

const function makeClassContents(class: classes.Class)
	local includes: { [string]: true } = {}

	const superclassBody = makeSuperclass(class, includes)
	const superclassMacro = makeClassDeclarationMacro(class)
	const implementationBody = makeClassImplementationBody(class, includes)

	includes[`"gargantuan/classes/{class.name}.hpp"`] = nil
	local rendered = { "// This file was procedurally @generated by Gargantuan. Do not edit!\n" }
	for include in orderedPairs(includes) do
		table.insert(rendered, `#include {include} // IWYU pragma: export`)
	end

	tableext.extend(rendered, superclassBody)
	table.insert(rendered, renderMacroBody(`G_DECL_{string.upper(class.name)}`, superclassMacro))
	table.insert(rendered, renderMacroBody(`G_IMPL_{string.upper(class.name)}`, implementationBody))

	return table.concat(rendered, "\n")
end

for _, target in TARGETS do
	if not fs.exists(target.output) then
		fs.createDirectory(target.output, { makeParents = true })
	end

	for _, entry in fs.listDirectory(target.input) do
		if entry.type == "file" and entry.name ~= ".config.luau" and string.find(entry.name, ".luau$") ~= nil then
			local inputFilepath = path.join(target.input, entry.name)
			local class: classes.Class = luau.loadModule(inputFilepath)

			local outputBasename = string.sub(entry.name, 1, -5)
			local outputFilepath = path.join(target.output, outputBasename .. "hpp")
			print(`Writing {path.format(outputFilepath)}`)
			fs.writeStringToFile(outputFilepath, makeClassContents(class))
		end
	end
end
