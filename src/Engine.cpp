#include "gargantuan/Engine.hpp"
#include "gargantuan/Log.hpp"
#include "gargantuan/Profiler.hpp"
#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/classes/FileLink.hpp"
#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/classes/Script.hpp"
#include "gargantuan/filesystem/Paths.hpp"
#include "gargantuan/render/Renderer.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"
#include "gargantuan/services/UserInputService.hpp"
#include "gargantuan/services/Workspace.hpp"

#include <SDL3/SDL.h>
#include <box3d/box3d.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <lua.h>
#include <memory>

namespace gargantuan {
	Engine::Engine(std::shared_ptr<gargantuan::DataModel> game, BaseRenderer *renderer)
		: DataModel(game), Renderer(renderer), Script(new class ScriptEngine(game)),
		  Workspace(GetService<gargantuan::Workspace>()),
		  WorldRoot(std::static_pointer_cast<gargantuan::WorldRoot>(Workspace)),
		  RunService(GetService<gargantuan::RunService>()), ProcessService(GetService<gargantuan::ProcessService>()),
		  UserInputService(GetService<gargantuan::UserInputService>()) {

		auto descendantAdded = [this](std::shared_ptr<Instance> inst) {
			if (auto script = std::dynamic_pointer_cast<gargantuan::Script>(inst)) {
				this->Script->ScriptQueue.insert(script);
				inst->Destroying->Once([ScriptEngine = this->Script, script](std::monostate _) {
					if (ScriptEngine->ScriptQueue.contains(script)) ScriptEngine->ScriptQueue.erase(script);
				});
			}

			if (auto link = std::dynamic_pointer_cast<gargantuan::FileLink>(inst)) {
				auto relativePath = link->GetPath();
				auto absolutePath = std::filesystem::absolute(this->DataModel->Root / relativePath);
				LOG_INFO(
					App,
					"Got file link: %s, %s %s %s",
					inst->GetClassName().c_str(),
					inst->GetFullName().c_str(),
					Paths::ToUtf8(absolutePath).c_str(),
					relativePath.c_str()
				);
				link->Synchronize(absolutePath);
			}
		};

		auto descendantRemoved = [this](std::shared_ptr<Instance> inst) {
			if (auto script = std::static_pointer_cast<gargantuan::Script>(inst);
				script && Script->ScriptQueue.contains(script)) {
				Script->ScriptQueue.erase(script);
			}
		};

		DataModel->DescendantAdded->Connect(descendantAdded);
		DataModel->DescendantRemoved->Connect(descendantRemoved);
		for (auto &descendant : DataModel->GetDescendants()) {
			descendantAdded(descendant);
		}

		LOG_INFO(App, "Constructed engine");
	}

	void Engine::Destroy() {
		LOG_INFO(App, "Destroying engine");
		Renderer->Destroy();
		WorldRoot->KillWorld();
	}

	float Engine::GetDeltaTime() {
		return (CurrentTick - LastTick) / 1000.0f;
	}

	void Engine::Step() {
		if (!ProcessService->Alive) return;

		CurrentTick = SDL_GetTicks();
		if (LastTick == 0) LastTick = CurrentTick;
		float deltaTime = GetDeltaTime();

		{
			G_PROFILE("Main Thread");

			{
				G_PROFILE("Events");

				SDL_Event event;
				while (SDL_PollEvent(&event)) {
					switch (event.type) {
					case SDL_EVENT_WINDOW_RESIZED: {
						auto window = SDL_GetWindowFromEvent(&event);
						if (!window) break;

						int width, height;
						SDL_GetWindowSizeInPixels(window, &width, &height);
						Renderer->Resize(width, height);

						Workspace->GetCurrentCamera()->SetViewportSize(Vector2(width, height));

						continue;
					}

					case SDL_EVENT_QUIT:
						LOG_INFO(App, "Stopping engine");
						ProcessService->Alive = false;
						return;
					}

					UserInputService->ProcessEvent(event);
					Workspace->GetCurrentCamera()->OnEvent(event);
				}
			}

			{
				G_PROFILE("Simulation");
				RunService->PreSimulation->Fire(deltaTime);
				WorldRoot->StepPhys(deltaTime);
				Workspace->GetCurrentCamera()->Step(deltaTime);
				RunService->PostSimulation->Fire(deltaTime);
			}

			{
				G_PROFILE("PreRender");
				RunService->PreRender->Fire(deltaTime);
			}

			{
				G_PROFILE("Draw");
				Renderer->Draw({
					.WorldRoot = WorldRoot,
					.Camera = Workspace->GetCurrentCamera(),
				});
			}

			{
				G_PROFILE("Scripts");
				Script->Step();
			}
		}

		G_PROFILE_FRAME();

		LastTick = CurrentTick;
	}
}
