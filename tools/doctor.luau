local args = require("@std/args")
local colors = require("@std/colors")
local env = require("@std/env")
local process = require("@std/process")

const parsed = args.parse("doctor", "checks for dependencies required to compile the engine")
	:simple(args.flag("--fix", "try to fix these issues"))

const hasHomebrew = process.run({ program = "brew", args = { "--version" } }).ok

const function tryInstall(package: string)
	if not parsed.flags.fix then
		return false
	end

	if hasHomebrew then
		print(colors.style.bold(`Installing {package} using Homebrew`))
		return process.run({
			program = "brew",
			args = { "install", package },
			stdio = "Inherit",
		}).ok
	end

	return false
end

do
	const issues = {}

	if not process.run({ program = "cmake", args = { "--version" } }).ok and not tryInstall("cmake") then
		table.insert(issues, "Missing CMake. See https://cmake.org/download/ for downloads.")
	end

	if
		not process.run({ program = "ninja", args = { "--version" } }).ok
		and env.os ~= "Windows"
		and not tryInstall("ninja")
	then
		table.insert(issues, "Missing a supported generator. See https://ninja-build.org/ for Unix systems.")
	end

	if not process.run({ program = "ccache", args = { "--version" } }).ok and not tryInstall("ccache") then
		warn("Missing CCache. See https://ccache.dev/download.html for downloads.")
	end

	if not process.run({ program = "glslc", args = { "--version" } }).ok and not tryInstall("glslc") then
		table.insert(issues, "Missing glslc. See https://github.com/google/shaderc/tags for downloads.")
	end

	if env.os ~= "MacOS" and env.os ~= "Linux" and env.os ~= "Windows" then
		table.insert(issues, "Unsupported operating system. Gargantuan can only be built in MacOS, Linux, and Windows.")
	end

	const numIssues = #issues
	if numIssues == 0 then
		print(colors.bold.green("No issues were found!"))
		process.exit(0)
	else
		print(colors.bold.red(`Found {#issues} issue{numIssues == 1 and "" or "s"}:`))
		for _, issue in issues do
			print(colors.style.dim(colors.red("* ") .. issue))
		end
		process.exit(1)
	end
end
