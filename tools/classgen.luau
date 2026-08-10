const classes = require("@tools/classes")
const cli = require("@batteries/cli")
const fs = require("@std/fs")
const luau = require("@std/luau")
const orderedPairs = require("@tools/utils/orderedPairs")
const path = require("@std/path")
const StringBuilder = require("@tools/utils/StringBuilder")
const types = require("@tools/types")
local process = require("@std/process")
local stringext = require("@std/stringext")

type Target = {
	input: path.Pathlike,
	headerImport: path.Pathlike,
	headerOutput: path.Pathlike,
	implOutput: path.Pathlike,
}

type GenerationState = {
	class: classes.Class,
	target: Target,
	superclass: string?,

	definitionProperties: StringBuilder.StringBuilder,
	definitionMethods: StringBuilder.StringBuilder,
	protectedBody: StringBuilder.StringBuilder,
	publicBody: StringBuilder.StringBuilder,

	includes: { [string]: true },
}

const TARGETS: { [string]: Target } = {
	classes = {
		input = "assets/classes",
		headerImport = "gargantuan/classes",
		headerOutput = "include/gargantuan/classes/generated",
		implOutput = "src/classes/generated",
	},
	services = {
		input = "assets/services",
		headerImport = "gargantuan/services",
		headerOutput = "include/gargantuan/services/generated",
		implOutput = "src/services/generated",
	},
}

const args = cli.parser()
args:add("force", "flag", { help = "whether to force regeneration", aliases = { "f" } })
args:parse({ ... })

const function pushTypeDependencies(state: GenerationState, type: types.Type)
	if type.cIncludes then
		for _, include in type.cIncludes do
			state.includes[include] = true
		end
	end
end

const function assembleProperties(state: GenerationState)
	if not state.class.properties then
		return
	end

	for key, property in orderedPairs(state.class.properties) do
		pushTypeDependencies(state, property.type)

		const keyString = string.format("%q", key)

		state.definitionProperties
			:line(`InstanceProperty({keyString})`)
			:tab()
			:line(`.SetReflectedTypedef({string.format("%q", property.type.luauType)})`)
			:line(`.SetUnmodified({property.unmodified})`)
		if property.serializable then
			state.definitionProperties:line(`.SetSerializable()`)
		end

		if property.overrideRead or property.overrideWrite then
			if property.overrideRead then
				state.publicBody:line(`[[nodiscard]] {property.type.cType} {property.overrideRead}() const;`)
				state.definitionProperties:line(`.UseRead<&{state.class.name}::{property.overrideRead}>()`)
			end

			if property.overrideWrite then
				state.publicBody:line(`void {property.overrideWrite}({property.type.cType} value);`)
				state.definitionProperties:line(`.UseWrite<&{state.class.name}::{property.overrideWrite}>()`)
			end
		else
			const getIdentifier, setIdentifier = `Get{key}`, `Set{key}`
			state.protectedBody:line(`{property.type.cType} {key} = {property.unmodified};`)

			state.publicBody
				:line(`[[nodiscard]] {property.type.cType} {getIdentifier}() const \{`)
				:tab()
				:line(`return {key};`)
				:untab()
				:line(`}`)
				:newline()
				:line(`void {setIdentifier}({property.type.cType} value) \{`)
				:tab()
				:line(`GetPropertyChangedSignal({keyString})->Fire(\{});`)
				:line(`{key} = value;`)
				:untab()
				:line(`}`)
				:newline()

			state.definitionProperties:line(`.UseRead<&{state.class.name}::{getIdentifier}>()`)
			state.definitionProperties:line(`.UseWrite<&{state.class.name}::{setIdentifier}>()`)
		end

		state.definitionProperties:line(".IntoPair(),"):untab()
	end
end

const function assembleSignals(state: GenerationState)
	if not state.class.signals then
		return
	end

	for key, signal in orderedPairs(state.class.signals) do
		const signalType = if signal.args then types.Signal(table.unpack(signal.args)) else types.Signal()
		pushTypeDependencies(state, signalType)

		const innerType = string.match(signalType.cType, "std::shared_ptr<(.+)>")

		const typedefIdentifier = `{key}Signal`
		state.publicBody
			:line(`typedef {innerType} {typedefIdentifier};`)
			:line(`std::shared_ptr<{typedefIdentifier}> {key} = std::make_shared<{typedefIdentifier}>();`)

		const keyString = string.format("%q", key)
		state.definitionProperties
			:line(`InstanceProperty({keyString})`)
			:tab()
			:line(`.SetReflectedTypedef({string.format("%q", signalType.luauType)})`)
			:line(`.SetSignal()`)
			:line(`.UseRead<&{state.class.name}::{key}>()`)
			:line(`.IntoPair(),`)
			:untab()
	end
end

const function assembleMethods(state: GenerationState)
	if not state.class.methods then
		return
	end

	for key, method in orderedPairs(state.class.methods) do
		state.definitionMethods:push("{"):push(string.format("%q", key)):push(", ")

		if method.asLuau then
			state.publicBody:line(`static int {key}(lua_State* L, Instance* inst);`)

			state.definitionMethods
				:push("Method{.Call = &")
				:push(state.class.name)
				:push("::")
				:push(key)
				:push(", .Signature = ")
				:push(string.format("%q", method.luauSignature or "(...unknown) -> (...unknown)"))
				:push("}")
		else
			state.publicBody:push(method.returns and method.returns.cType or "void"):push(" "):push(key):push("(")
			if method.params then
				for index, param in method.params do
					pushTypeDependencies(state, param.type)
					state.publicBody:push(param.type.cType):push(" "):push(param.name)
					if index ~= #method.params then
						state.publicBody:push(", ")
					end
				end
			end
			state.publicBody:push(");"):newline()

			state.definitionMethods:push("Method::fromMember<&"):push(state.class.name):push("::"):push(key):push(">()")
		end

		state.definitionMethods:push("},"):newline()
	end
end

const function assembleHeaderFile(state: GenerationState)
	const macroBody = StringBuilder:new()
		:tabHalf()
		:line("public:")
		:tabHalf()
		:line("static const InstanceClassDefinition CLASS_DEFINITION;")
		:untabHalf()
		:line("protected:")
		:tabHalf()
		:pushIndented(stringext.trim(state.protectedBody:build()))
		:untabHalf()
		:newline()
		:line("public:")
		:tabHalf()
		:pushIndented(stringext.trim(state.publicBody:build()))
		:untabHalf()
		:newline()

	const headerBody = StringBuilder:new()
		:line("// This file was procedurally @generated by Gargantuan. Do not edit!")
		:newline()
		:line("#pragma once")
		:newline()

	-- Can't include ourselves here
	state.includes[`"gargantuan/classes/{state.class.name}.hpp"`] = nil
	state.includes[`"gargantuan/services/{state.class.name}.hpp"`] = nil
	for include in orderedPairs(state.includes) do
		headerBody:push("#include "):push(include):push(" // IWYU pragma: keep"):newline()
	end

	headerBody
		:newline()
		:line(`#define I_{state.class.name} \\`)
		:tab()
		:pushIndented(string.gsub(stringext.trim(macroBody:build()), "\n", "\\\n"))

	return headerBody
end

const function assembleImplFile(state: GenerationState)
	const output = StringBuilder:new()

	output:line("// This file was procedurally @generated by Gargantuan. Do not edit!"):newline()

	const includePath = path.join(state.target.headerImport, state.class.name .. ".hpp")
	output:line(`#include {string.format("%q", path.format(includePath))}`)

	output:line("namespace gargantuan {"):tab()

	output:line(`const InstanceClassDefinition {state.class.name}::CLASS_DEFINITION\{`):tab()
	output:line(`.ClassName = {string.format("%q", state.class.name)},`)
	if not state.class.abstract then
		output
			:line(`.Constructor = []() -> std::shared_ptr<Instance> \{`)
			:tab()
			:pushIndented(`return std::make_shared<gargantuan::`)
			:push(state.class.name)
			:push(`>();`)
			:newline()
			:untab()
			:line("},")
	end
	output:line(`.Superclass = {state.superclass and string.format("%q", state.superclass) or "std::nullopt"},`)
	output
		:line(".Properties = {")
		:tab()
		:pushIndented(stringext.trim(state.definitionProperties:build()))
		:newline()
		:untab()
		:pushIndented("},")
		:newline()
	output
		:line(".Methods = {")
		:tab()
		:pushIndented(stringext.trim(state.definitionMethods:build()))
		:newline()
		:untab()
		:pushIndented("},")
		:newline()
	output:untab():line("};")

	output
		:line("namespace InstanceClassRegistry::__IGNORE__ {")
		:tab()
		:pushIndented("bool ")
		:push(state.class.name)
		:push(" = []() {")
		:newline()
		:tab()
		:pushIndented(`gargantuan::InstanceClassRegistry::Register<gargantuan::{state.class.name}>`)
		:push(`(gargantuan::{state.class.name}::CLASS_DEFINITION);`)
		:newline()
		:line("return true;")
		:untab()
		:line("}();")
	output:untab():line("};"):untab():line("};")

	return output
end

const function assembleFile(target: Target, inputFilepath: path.Pathlike)
	const class: classes.Class = luau.loadModule(inputFilepath)

	local inputLastModified = fs.metadata(inputFilepath).modified:toSeconds()

	local headerOutput = path.join(target.headerOutput, class.name .. ".hpp")
	local isHeaderUpdated = not args:has("force")
		and fs.exists(headerOutput)
		and fs.metadata(headerOutput).modified:toSeconds() >= inputLastModified

	local implOutput = path.join(target.implOutput, class.name .. ".cpp")
	local isImplUpdated = not args:has("force")
		and fs.exists(implOutput)
		and fs.metadata(implOutput).modified:toSeconds() >= inputLastModified

	if isHeaderUpdated and isImplUpdated then
		print(`Skipping {class.name}`)
		return
	end

	print(`Assembling {class.name}`)

	const state: GenerationState = {
		class = class,
		target = target,
		superclass = if class.root then nil else class.extends or "Instance",

		definitionProperties = StringBuilder.new(),
		definitionMethods = StringBuilder.new(),
		protectedBody = StringBuilder.new(),
		publicBody = StringBuilder.new(),

		includes = {
			['"gargantuan/InstanceProperty.hpp"'] = true,
			['"gargantuan/reflection/InstanceClassRegistry.hpp"'] = true,
		},
	}

	if state.superclass then
		pushTypeDependencies(state, types.Instance(state.superclass))
	end

	assembleMethods(state)
	assembleProperties(state)
	assembleSignals(state)

	if not isHeaderUpdated then
		fs.writeStringToFile(headerOutput, assembleHeaderFile(state):build())
		process.run({ "clang-format", "-i", path.format(headerOutput) })
	end

	if not isImplUpdated then
		fs.writeStringToFile(implOutput, assembleImplFile(state):build())
		process.run({ "clang-format", "-i", path.format(implOutput) })
	end
end

do
	for _, target in orderedPairs(TARGETS) do
		if not fs.exists(target.headerOutput) then
			fs.createDirectory(target.headerOutput, { makeParents = true })
		end

		if not fs.exists(target.implOutput) then
			fs.createDirectory(target.implOutput, { makeParents = true })
		end

		for _, entry in fs.listDirectory(target.input) do
			if entry.type == "file" and entry.name ~= ".config.luau" and string.find(entry.name, ".luau$") ~= nil then
				assembleFile(target, path.join(target.input, entry.name))
			end
		end
	end
end
