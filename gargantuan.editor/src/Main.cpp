#include "gargantuan.core/Engine.hpp"
#include "gargantuan.core/Log.hpp"
#include "gargantuan.core/classes/DataModel.hpp"
#include "gargantuan.core/classes/Script.hpp"
#include "gargantuan.core/datatypes/Vector2.hpp"
#include "gargantuan.core/filesystem/DiskFilesystem.hpp"
#include "gargantuan.core/filesystem/Project.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_log.h>
#include <argparse/argparse.hpp>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <memory>

int main(int argc, char *argv[]) {
	argparse::ArgumentParser program("gargantuan");
	program.add_description("An independent game engine for Roblox developers");
	program.add_group("Gargantuan");
	program.add_argument("--project").help("path of a project directory to be loaded").default_value("-");
	program.add_argument("--script").help("path of a Luau script to be loaded").default_value("-");
	program.add_argument("--headless").flag().help("whether to disable the renderer");
	program.add_group("Logging");
	program.add_argument("--noansi").flag().help("disable ansi logs");
	program.add_argument("--nopretty").flag().help("whether to print json structured logs");

	SDL_Init(SDL_INIT_VIDEO);
	std::atexit(SDL_Quit);

	try {
		program.parse_args(argc, argv);
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		std::exit(1);
	}

	gargantuan::LogContext logContext{
		.EnableAnsi = !program.is_used("--noansi"),
		.EnablePretty = !program.is_used("--nopretty"),
	};
	SDL_SetLogPriorities(SDL_LOG_PRIORITY_TRACE);
	SDL_SetLogPriority(gargantuan::LogCategory::App, SDL_LOG_PRIORITY_DEBUG);
	SDL_SetLogPriority(gargantuan::LogCategory::Lua, SDL_LOG_PRIORITY_TRACE);
	SDL_SetLogOutputFunction(gargantuan::GetLogOutputFunction(&logContext), &logContext);

	LOG_INFO(App, "Gargantuan start");

	// Constructing the renderer
	gargantuan::Vector2 viewportSize(720, 540);
	gargantuan::BaseRenderer *renderer = nullptr;
	if (program.is_used("--headless")) {
		renderer = new gargantuan::HeadlessRenderer(viewportSize);
	} else {
		try {
			renderer = new gargantuan::SDLRenderer(viewportSize);
		} catch (std::exception &e) {
			LOG_CRITICAL(App, "Failed to construct SDL3 renderer: %s", e.what());
			std::exit(1);
		}
	}

	// Constructing the engine
	gargantuan::Engine *engine = nullptr;

	if (program.is_used("--project")) {
		// Projects
		auto root = std::filesystem::path(program.get<std::string>("--project"));
		try {
			auto fs = new gargantuan::DiskFilesystem(root);
			auto project = gargantuan::Project::fromExisting(fs);
			auto game = project.DeserializeGame();
			engine = new gargantuan::Engine(game, renderer);
		} catch (std::exception &e) {
			LOG_CRITICAL(App, "Failed to open project %s: %s", root.c_str(), e.what());
			return 1;
		}
	} else if (program.is_used("--script")) {
		auto path = program.get<std::string>("--script");

		try {
			auto game = std::make_shared<gargantuan::DataModel>();
			engine = new gargantuan::Engine(game, renderer);

			auto script = gargantuan::ScriptFromFile<gargantuan::Script>(path.c_str());
			script->SetParent(engine->Workspace);
		} catch (std::exception &e) {
			LOG_CRITICAL(App, "%s", e.what());
			std::exit(1);
		}
	} else {
		LOG_CRITICAL(App, "Missing --project or --script to load");
		std::exit(1);
	}

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
