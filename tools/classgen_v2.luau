const fs = require("@std/fs")
const luau = require("@std/luau")
const path = require("@std/path")
local stringext = require("@std/stringext")
const classes = require("@tools/classes")
const types = require("@tools/types")
const orderedPairs = require("@tools/utils/orderedPairs")
const StringBuilder = require("@tools/utils/StringBuilder")

type GenerationState = {
	class: classes.Class,
	headerImport: path.Pathlike,
	superclass: string?,
	superclassIdentifier: string,

	definitionProperties: StringBuilder.StringBuilder,
	definitionMethods: StringBuilder.StringBuilder,
	protectedBody: StringBuilder.StringBuilder,
	publicBody: StringBuilder.StringBuilder,

	outputIncludes: { [string]: true },
	forwardStatements: { [string]: true },
}

type Target = {
	input: path.Pathlike,
	headerImport: path.Pathlike,
	headerOutput: path.Pathlike,
	implOutput: path.Pathlike,
}

const TARGETS: { [string]: Target } = {
	classes = {
		input = "assets/classes",
		headerImport = "gargantuan/classes/generated",
		headerOutput = "include/gargantuan/classes/generated",
		implOutput = "src/classes/generated",
	},
	services = {
		input = "assets/services",
		headerImport = "gargantuan/classes/generated",
		headerOutput = "include/gargantuan/services/generated",
		implOutput = "src/classes/generated",
	},
}

const function pushTypeDependencies(state: GenerationState, type: types.Type)
	if type.cForwardStatement then
		state.forwardStatements[type.cForwardStatement] = true
	end

	if type.cIncludes then
		for _, include in type.cIncludes do
			state.outputIncludes[include] = true
		end
	end
end

const function prepareGeneration(class: classes.Class, headerImport: path.Pathlike): GenerationState
	const state: GenerationState = {
		class = class,
		headerImport = headerImport,
		superclass = if class.root then nil else class.extends or "Instance",
		superclassIdentifier = `__CODEGEN__{class.name}`,

		definitionProperties = StringBuilder.new(),
		definitionMethods = StringBuilder.new(),
		protectedBody = StringBuilder.new(),
		publicBody = StringBuilder.new(),

		outputIncludes = {
			['"gargantuan/InstanceProperty.hpp"'] = true,
			['"gargantuan/reflection/InstanceClassRegistry.hpp"'] = true,
		},
		forwardStatements = { [`class {class.name};`] = true },
	}

	if state.superclass then
		pushTypeDependencies(state, types.IsA(state.superclass))
	end

	return state
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
				state.publicBody:line(`{property.type.cType} {property.overrideRead}();`)
				state.definitionProperties:line(`.UseRead<&{state.superclassIdentifier}::{property.overrideRead}>()`)
			end

			if property.overrideWrite then
				state.publicBody:line(`void {property.overrideWrite}({property.type.cType} value);`)
				state.definitionProperties:line(`.UseWrite<&{state.superclassIdentifier}::{property.overrideWrite}>()`)
			end
		else
			const getIdentifier, setIdentifier = `Get{key}`, `Set{key}`
			state.protectedBody:line(`{property.type.cType} {key} = {property.unmodified};`)

			state.publicBody
				:line(`{property.type.cType} {getIdentifier}() \{`)
				:tab()
				:line(`return {key};`)
				:untab()
				:line(`}`)
				:newline()
				:line(`void {setIdentifier}({property.type.cType} value) \{`)
				:tab()
				:line(`GetPropertyChangedSignal({keyString})->Fire();`)
				:line(`{key} = value;`)
				:untab()
				:line(`}`)
				:newline()

			state.definitionProperties:line(`.UseRead<&{state.superclassIdentifier}::{getIdentifier}>()`)
			state.definitionProperties:line(`.UseWrite<&{state.superclassIdentifier}::{setIdentifier}>()`)
		end

		state.definitionProperties:line(".IntoPair(),"):untab()
	end
end

const function assembleSignals(state: GenerationState)
	if not state.class.signals then
		return
	end

	pushTypeDependencies(state, types.Signal)

	for key, signal in orderedPairs(state.class.signals) do
		local cType: string, luauGenericType: string

		const numArgs = signal.args and #signal.args or 0
		if not signal.args or numArgs == 0 then
			state.outputIncludes["<variant>"] = true
			cType = "std::monostate"
			luauGenericType = "()"
		elseif numArgs == 1 then
			const arg = signal.args[1]
			pushTypeDependencies(state, arg)
			cType = arg.cType
			luauGenericType = arg.luauType
		else
			state.outputIncludes["<tuple>"] = true
			cType = "std::tuple<"
			luauGenericType = ""
			for index, arg in signal.args do
				pushTypeDependencies(state, arg)
				cType ..= arg.cType
				luauGenericType ..= arg.luauType
				if index ~= numArgs then
					cType ..= ", "
					luauGenericType ..= ", "
				end
			end
			cType ..= ">"
		end

		const typedefIdentifier = `{key}Signal`
		state.publicBody
			:line(`typedef gargantuan::Signal<{cType}> {typedefIdentifier};`)
			:line(`{typedefIdentifier} {key} = std::make_shared<{typedefIdentifier}>();`)

		const keyString = string.format("%q", key)
		state.definitionProperties
			:line(`InstanceProperty({keyString})`)
			:tab()
			:line(`.SetReflectedTypedef({string.format("%q", `Signal<{luauGenericType}>`)})`)
			:line(`.SetSignal()`)
			:line(`.SetRead<&{state.superclassIdentifier}::{key}>()`)
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
				:push(state.superclassIdentifier)
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

			state.definitionMethods
				:push("Method::fromMember<&")
				:push(state.superclassIdentifier)
				:push("::")
				:push(key)
				:push(">()")
		end

		state.definitionMethods:push("},"):newline()
	end
end

const function assembleHeader(state: GenerationState)
	const output = StringBuilder:new()

	output:line("// This file was procedurally @generated by Gargantuan. Do not edit!"):newline()

	-- Can't include ourselves here
	state.outputIncludes[`"gargantuan/classes/{state.class.name}.hpp"`] = nil
	state.outputIncludes[`"gargantuan/services/{state.class.name}.hpp"`] = nil
	for include in orderedPairs(state.outputIncludes) do
		output:push("#include "):push(include):newline()
	end

	output:newline():line("namespace gargantuan {"):tab()

	-- Superclass codegen
	do
		for stat in orderedPairs(state.forwardStatements) do
			output:line(stat)
		end

		output:pushIndented("class "):push(state.superclassIdentifier)
		if state.superclass then
			output:push(": public __CODEGEN__"):push(state.superclass)
		end
		output:push(" {"):newline():tab()

		output
			:untabHalf()
			:line("protected:")
			:tabHalf()
			:pushIndented(stringext.trim(state.protectedBody:build()))
			:newline()
		output:untabHalf():line("public:"):tabHalf():pushIndented(stringext.trim(state.publicBody:build())):newline()

		output:line("static const InstanceClassDefinition CLASS_DEFINITION;"):tab()

		output:untab():line("};"):untab():line("};")
	end

	-- INSTANCE_X macro codegen
	output
		:untab()
		:line("};")
		:newline()
		:push("#define ")
		:push("I_")
		:push(state.class.name)
		:push(" ")
		:push(state.class.name)
		:push(": public ")
		:push(state.superclassIdentifier)

	return output
end

const function assembleImplementation(state: GenerationState)
	const output = StringBuilder:new()

	output:line("// This file was procedurally @generated by Gargantuan. Do not edit!"):newline()

	output:line(`#include {string.format("%q", path.format(state.headerImport))}`)
	output:line("namespace gargantuan {"):tab()

	output:line(`const InstanceClassDefinition {state.superclassIdentifier}::CLASS_DEFINITION\{`):tab()
	output:line(`.ClassName = {string.format("%q", state.class.name)},`)
	output
		:line(`.Constructor = []() -> std::shared_ptr<Instance> \{`)
		:tab()
		:pushIndented(`return std::make_shared<gargantuan::`)
		:push(state.class.name)
		:push(`>();`)
		:newline()
		:untab()
		:line("},")
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
		:push(`(gargantuan::{state.superclassIdentifier}::CLASS_DEFINITION);`)
		:newline()
		:line("return true;")
		:untab()
		:line("}();")
	output:untab():line("};"):untab():line("};")

	return output
end

for _, target in orderedPairs(TARGETS) do
	if not fs.exists(target.headerOutput) then
		fs.createDirectory(target.headerOutput, { makeParents = true })
	end

	if not fs.exists(target.implOutput) then
		fs.createDirectory(target.implOutput, { makeParents = true })
	end

	for _, entry in fs.listDirectory(target.input) do
		if entry.type == "file" and entry.name ~= ".config.luau" and string.find(entry.name, ".luau$") ~= nil then
			local inputFilepath = path.join(target.input, entry.name)
			local class: classes.Class = luau.loadModule(inputFilepath)

			print(`Assembling {class.name}`)

			local outputBasename = string.sub(entry.name, 1, -5)
			local headerImport = path.join(target.headerImport, outputBasename .. "hpp")

			const state = prepareGeneration(class, headerImport)
			assembleMethods(state)
			assembleProperties(state)
			assembleSignals(state)

			local headerOutput = path.join(target.headerOutput, outputBasename .. "hpp")
			fs.writeStringToFile(headerOutput, assembleHeader(state):build())

			local implOutput = path.join(target.implOutput, outputBasename .. "cpp")
			fs.writeStringToFile(implOutput, assembleImplementation(state):build())
		end
	end
end
