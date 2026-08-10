#include "gargantuan/Engine.hpp"
#include "gargantuan/Log.hpp"
#include "gargantuan/assets/InstanceSerialization.hpp"
#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/classes/Script.hpp"
#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/filesystem/DiskFilesystem.hpp"
#include "gargantuan/filesystem/Project.hpp"
#include "gargantuan/render/Renderer.hpp"

#include <SDL3/SDL.h>
#include <argparse/argparse.hpp>
#include <magic_enum/magic_enum.hpp>

#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

using namespace gargantuan;

Engine *ConstructProject(std::string path, BaseRenderer *renderer) {
	auto root = std::filesystem::path(path);
	try {
		auto fs = new DiskFilesystem(root);
		auto project = Project::fromExisting(fs);
		auto game = project.DeserializeGame();
		return new Engine(game, renderer);
	} catch (std::exception &e) {
		LOG_CRITICAL(App, "%s", e.what());
		std::exit(1);
	}
}

Engine *ConstructScript(std::string path, BaseRenderer *renderer) {
	try {
		auto game = std::make_shared<DataModel>();
		auto engine = new Engine(game, renderer);

		auto script = ScriptFromFile<Script>(path.c_str());
		script->SetParent(engine->Workspace);

		return engine;
	} catch (std::exception &e) {
		LOG_CRITICAL(App, "%s", e.what());
		std::exit(1);
	}
}

Engine *ConstructInstance(std::string path, BaseRenderer *renderer) {
	SDL_PathInfo pathInfo;
	if (!SDL_GetPathInfo(path.c_str(), &pathInfo)) {
		LOG_CRITICAL(App, "Failed to get path info for %s: %s", path.c_str(), SDL_GetError());
		std::exit(1);
	} else if (pathInfo.type != SDL_PATHTYPE_FILE) {
		auto typeNameView = magic_enum::enum_name(pathInfo.type);
		auto typeName = std::string(typeNameView.data(), typeNameView.size());
		LOG_CRITICAL(App, "Expected %s to be an instance file, got %s", path.c_str(), typeName.c_str());
		std::exit(1);
	}

	InstanceSerialization::InstanceFormat format;
	if (path.ends_with(".instance.json")) {
		format = InstanceSerialization::InstanceFormat::Json;
	} else if (path.ends_with(".instance.bin")) {
		format = InstanceSerialization::InstanceFormat::Binary;
	} else {
		LOG_CRITICAL(App, "Unable to infer instance format of %s", path.c_str());
		std::exit(1);
	}

	std::ifstream fileStream(path);
	if (!fileStream.is_open()) {
		LOG_CRITICAL(App, "Failed to open instance file %s", path.c_str());
		std::exit(1);
	}

	auto deserialized = InstanceSerialization::Deserialize(format, fileStream);
	if (!deserialized.Ok) {
		LOG_CRITICAL(App, "Failed to deserialize instance file %s:", path.c_str());
		for (auto &reason : deserialized.Errors) {
			LOG_CRITICAL(App, "* %s", reason.c_str());
		}
		std::exit(1);
	}

	std::shared_ptr<DataModel> game;

	auto instance = deserialized.Instance;
	if (auto maybeGame = static_pointer_cast<DataModel>(instance)) {
		game = maybeGame;
	} else {
		game = std::make_shared<DataModel>();
		instance->SetParent(game->GetService("Workspace"));
	}

	return new Engine(game, renderer);
}

int main(int argc, char *argv[]) {
	argparse::ArgumentParser program("gargantuan");
	program.add_description("An independent game engine for Roblox developers");
	program.add_group("Targets");
	program.add_argument("--project").help("path of a project directory to be loaded").default_value("-");
	program.add_argument("--script").help("path of a Luau script to be loaded").default_value("-");
	program.add_argument("--instance").help("path ofg an instance file to be loaded").default_value("-");
	program.add_group("Engine");
	program.add_argument("--headless").flag().help("whether to disable the renderer");
	program.add_argument("--enable_roblox_compat").flag().help("use roblox api compatibility (overrides projects)");
	program.add_group("Logging");
	program.add_argument("--no_ansi").flag().help("disable ansi logs");
	program.add_argument("--no_pretty").flag().help("whether to print json structured logs");
	program.add_argument("--app_log_level").help("log level for gargantuan itself").default_value("trace");
	program.add_argument("--lua_log_level").help("log level for lua runtime").default_value("trace");
	program.add_argument("--sdl_log_level").help("log level for sdl library").default_value("trace");

	try {
		program.parse_args(argc, argv);
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		std::exit(1);
	}

	LogContext logContext{
		.EnableAnsi = !program.is_used("--no_ansi"),
		.EnablePretty = !program.is_used("--no_pretty"),
	};
	SDL_SetLogPriorities(SDL_LOG_PRIORITY_TRACE);
	SDL_SetLogPriority(LogCategory::App, SDL_LOG_PRIORITY_TRACE);
	SDL_SetLogPriority(LogCategory::Lua, SDL_LOG_PRIORITY_TRACE);
	SDL_SetLogOutputFunction(GetLogOutputFunction(&logContext), &logContext);

	LOG_INFO(App, "Gargantuan start");

	int hasProject = program.is_used("--project");
	int hasScript = program.is_used("--script");
	int hasInstance = program.is_used("--instance");
	if (hasProject + hasScript + hasInstance == 0) {
		LOG_CRITICAL(App, "No target provided, specify one of: --project, --script, or --instance");
		std::exit(1);
	} else if (hasProject + hasScript + hasInstance > 1) {
		LOG_CRITICAL(App, "Too many targets provided, specify one of: --project, --script, or --instance");
		std::exit(1);
	}

	Vector2 viewportSize(720, 540);
	BaseRenderer *renderer = nullptr;

	std::atexit(SDL_Quit);

	if (program.is_used("--headless")) {
		SDL_Init(SDL_INIT_EVENTS);

		renderer = new HeadlessRenderer(viewportSize);
	} else {
		SDL_Init(SDL_INIT_VIDEO);

		try {
			renderer = new SDLRenderer(viewportSize);
		} catch (std::exception &e) {
			LOG_CRITICAL(App, "Failed to construct SDL3 renderer: %s", e.what());
			std::exit(1);
		}
	}

	auto engine = hasProject  ? ConstructProject(program.get<std::string>("--project"), renderer)
				  : hasScript ? ConstructScript(program.get<std::string>("--script"), renderer)
							  : ConstructInstance(program.get<std::string>("--instance"), renderer);

	LOG_INFO(App, "Starting engine loop");
	engine->ProcessService->Alive = true;
	try {
		while (engine->ProcessService->Alive) {
			engine->Step();
		}
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		std::exit(1);
	}

	auto exitCode = engine->ProcessService->ExitCode;
	engine->Destroy();
	std::exit(exitCode);
}
