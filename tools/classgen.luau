const fs = require("@std/fs")
const path = require("@std/path")
const process = require("@std/process")
const luau = require("@std/luau")
local tableext = require("@std/tableext")
local classes = require("@tools/classes")
local types = require("@tools/types")

const CLASSES_INPUT_PATH = path.join(process.cwd(), "assets/classes")
const CLASSES_OUTPUT_PATH = path.join(process.cwd(), "include/gargantuan/classes/generated")

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

const function makeClassDeclarationBody(class: classes.Class, includes: { [string]: true })
	local protectedBody = { "protected:" }
	local publicBody = { "public:" }

	if class.properties then
		for key, property in orderedPairs(class.properties) do
			pushIncludes(property.type, includes)
			if property.overrideRead or property.overrideWrite then
				if property.overrideRead then
					table.insert(publicBody, `{property.type.cType} {property.overrideRead}();`)
				end
				if property.overrideWrite then
					table.insert(publicBody, `{property.type.cType} {property.overrideWrite}();`)
				end
			else
				table.insert(protectedBody, `{property.type.cType} {key} = {property.unmodified};`)
				table.insert(publicBody, `{property.type.cType} Get{key}();`)
				table.insert(publicBody, `void Set{key}({property.type.cType} value);`)
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

				local signalTypedef = "std::tuple<"
				for index, type in signal.args do
					signalTypedef ..= type.cType
					pushIncludes(type, includes)
					if index ~= #signal.args then
						signalTypedef ..= ", "
					end
				end
				signalTypedef ..= ">"

				table.insert(publicBody, `typedef {signalTypedef} {key}Type;`)
				signalType = `{key}Type`
			else
				includes["<variant>"] = true
				signalType = "std::monostate"
			end

			includes['"gargantuan/datatypes/Signal.hpp"'] = true
			table.insert(
				publicBody,
				`std::shared_ptr<Signal<{signalType}>> {key} = std::make_shared<Signal<{signalType}>>();`
			)
		end
	end

	if class.methods then
		for key, method in orderedPairs(class.methods) do
			if method.asLuau then
				includes["<lua.h>"] = true
				table.insert(publicBody, `int {key}(lua_State* L, Instance* inst);`)
			else
				const parameters = {}
				if method.params then
					for i, parameter in method.params do
						pushIncludes(parameter.type, includes)
						parameters[i] = `{parameter.type.cType} {parameter.name}`
					end
				end
				const returnType = method.returns and method.returns.cType or "void"
				table.insert(publicBody, `{returnType} {key}({table.concat(parameters, ", ")});`)
			end
		end
	end

	const body = { "public:", "static const InstanceClassDefinition CLASS_DEFINITION;" }
	tableext.extend(body, protectedBody, publicBody)
	return body
end

const function makeClassImplementationBody(class: classes.Class, includes: { [string]: true })
	const body = {
		`const InstanceClassDefinition {class.name}::CLASS_DEFINITION \{`,
		`\t.ClassName = {class.name},`,
	}

	if not class.abstract then
		includes["<memory>"] = true
		table.insert(
			body,
			`\t.Constructor = []() -> std::make_shared<gargantuan::Instance> \{ return std::make_shared<gargantuan::{class.name}>(); },`
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
		includes['"gargantuan/datatypes/instances/InstanceProperty.hpp"'] = true
		for key, property in orderedPairs(class.properties) do
			table.insert(body, `\t\tInstanceProperty({string.format("%q", key)})`)
			if property.serializable then
				table.insert(body, `\t\t\t.Serializable()`)
			end
			table.insert(body, `\t\t\t.LuauType({string.format("%q", property.type.luauType)})`)
			if property.overrideRead or not property.overrideWrite then
				table.insert(body, `\t\t\t.UseReader<{`&{class.name}::{property.overrideRead or `Get{key}`}`}>()`)
			end
			if property.overrideWrite or not property.overrideRead then
				table.insert(body, `\t\t\t.UseWriter<{`&{class.name}::{property.overrideWrite or `Set{key}`}`}>()`)
			end
			table.insert(body, `\t\t\t.IntoPair(),`)
		end
	end
	if class.signals then
		includes['"gargantuan/datatypes/instances/InstanceProperty.hpp"'] = true
		for key, signal in orderedPairs(class.signals) do
			const signalArgs = table.create<<string>>(signal.args and #signal.args or 0)
			if signal.args then
				for i, arg in signal.args do
					signalArgs[i] = arg.luauType
				end
			end

			table.insert(body, `\t\tInstanceProperty({string.format("%q", key)})`)
			table.insert(body, `\t\t\t.Signal()`)
			table.insert(body, `\t\t\t.LuauType("Signal<{table.concat(signalArgs, ", ")}>")`)
			table.insert(body, `\t\t\t.UseReadonly<&{class.name}::{key}>()`)
			table.insert(body, `\t\t\t.AsPair(),`)
		end
	end
	table.insert(body, `\t},`)

	table.insert(body, `\t.Methods = \{`)
	if class.methods then
		for key, method in orderedPairs(class.methods) do
			if method.asLuau then
				table.insert(
					body,
					`\t\t\{"{key}", Method\{.Signature = {string.format("%q", method.luauSignature or "")}, .Call = {key}}},`
				)
			else
				table.insert(body, `\t\t\{"{key}", Method::fromMember<&{class.name}::{key}>()},`)
			end
		end
	end
	table.insert(body, `\t},`)

	table.insert(body, "}")
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
	local declarationBody = makeClassDeclarationBody(class, includes)
	const implementationBody = makeClassImplementationBody(class, includes)

	local rendered = { "// This file was procedurally @generated by Gargantuan. Do not edit!\n" }
	for include in orderedPairs(includes) do
		table.insert(rendered, `#include {include} // IWYU pragma: export`)
	end

	table.insert(rendered, "")
	table.insert(rendered, renderMacroBody(`G_DECL_{string.upper(class.name)}`, declarationBody))
	table.insert(rendered, renderMacroBody(`G_IMPL_{string.upper(class.name)}`, implementationBody))

	return table.concat(rendered, "\n")
end

if not fs.exists(CLASSES_OUTPUT_PATH) then
	fs.createDirectory(CLASSES_OUTPUT_PATH, { makeParents = true })
end

for _, entry in fs.listDirectory(CLASSES_INPUT_PATH) do
	if entry.type == "file" and entry.name ~= ".config.luau" and string.find(entry.name, ".luau$") ~= nil then
		local inputFilepath = path.join(CLASSES_INPUT_PATH, entry.name)
		local class: classes.Class = luau.loadModule(inputFilepath)

		local outputBasename = string.sub(entry.name, 1, -5)
		local outputFilepath = path.join(CLASSES_OUTPUT_PATH, outputBasename .. "hpp")
		print(`Writing {path.format(outputFilepath)}`)
		fs.writeStringToFile(outputFilepath, makeClassContents(class))
	end
end
