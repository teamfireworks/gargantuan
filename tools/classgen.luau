const args = require("@std/args")
const classes = require("@tools/classes")
const fs = require("@std/fs")
const luau = require("@std/luau")
const orderedPairs = require("@tools/utils/orderedPairs")
const path = require("@std/fs/path")
const StringBuilder = require("@tools/utils/StringBuilder")
const types = require("@tools/types")
local configuration = require("@tools/configuration")
local process = require("@std/process")
local str = require("@std/str")

type Target = {
	input: string,
	headerImport: string,
	headerOutput: string,
	implOutput: string,
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

const parsed = args.parse("classgen", "procedurally generates C++ files from Luau instance declarations")
	:simple(args.flag("--force", "whether to force regeneration"):aliases("-f"))

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
		:line(`typedef std::shared_ptr<gargantuan::{state.class.name}> Pointer;`)
		:line("static const InstanceClassDefinition CLASS_DEFINITION;")
		:untabHalf()
		:line("protected:")
		:tabHalf()
		:pushIndented(str.trim(state.protectedBody:build()))
		:untabHalf()
		:newline()
		:line("public:")
		:tabHalf()
		:pushIndented(str.trim(state.publicBody:build()))
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
		:pushIndented(string.gsub(str.trim(macroBody:build()), "\n", "\\\n"))

	return headerBody
end

const function assembleImplFile(state: GenerationState)
	const output = StringBuilder:new()

	output:line("// This file was procedurally @generated by Gargantuan. Do not edit!"):newline()

	const includePath = path.join(state.target.headerImport, state.class.name .. ".hpp")
	output:line(`#include {string.format("%q", includePath)}`)

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
		:pushIndented(str.trim(state.definitionProperties:build()))
		:newline()
		:untab()
		:pushIndented("},")
		:newline()
	output
		:line(".Methods = {")
		:tab()
		:pushIndented(str.trim(state.definitionMethods:build()))
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

const function isOutputUpdated(filepath: string, lastModifiedAt): boolean
	if parsed.flags.force then
		return false
	end

	const file = fs.find(filepath):try_file()
	if file then
		const metadata = file:metadata()
		return (metadata.modified_at ~= nil) and metadata.modified_at.unix_timestamp >= lastModifiedAt
	end

	return false
end

const function assembleFile(target: Target, declarationContents: fs.FileEntry)
	const classContents = fs.readfile(declarationContents.path)
	const classResult = luau.eval(classContents, { stdlib = "Seal", name = declarationContents.path })
	if typeof(classResult) == "error" then
		print(tostring(classResult))
		return
	end

	const class = classResult :: classes.Class
	const lastModifiedAt = declarationContents:metadata().modified_at

	local headerOutput = path.join(target.headerOutput, class.name .. ".hpp")
	local isHeaderUpdated = lastModifiedAt and isOutputUpdated(headerOutput, lastModifiedAt.unix_timestamp)

	local implOutput = path.join(target.implOutput, class.name .. ".cpp")
	local isImplUpdated = lastModifiedAt and isOutputUpdated(implOutput, lastModifiedAt.unix_timestamp)

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
		fs.writefile(headerOutput, assembleHeaderFile(state):build())
		process.run({ program = "clang-format", args = { "-i", headerOutput } })
	end

	if not isImplUpdated then
		fs.writefile(implOutput, assembleImplFile(state):build())
		process.run({ program = "clang-format", args = { "-i", implOutput } })
	end
end

do
	for _, target in orderedPairs(configuration.classgenTargets) do
		fs.dir.ensure(target.headerOutput)
		fs.dir.ensure(target.implOutput)

		const input = fs.dir.from(target.input)
		for _, entry in orderedPairs(input:entries()) do
			if entry.type == "File" and entry.name ~= ".config.luau" and string.find(entry.name, ".luau$") ~= nil then
				assembleFile(target, entry)
			end
		end
	end
end
