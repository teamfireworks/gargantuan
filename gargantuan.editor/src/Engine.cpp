#include "gargantuan.core/Engine.hpp"
#include "gargantuan.core/Log.hpp"
#include "gargantuan.core/Profiler.hpp"
#include "gargantuan.core/classes/DataModel.hpp"
#include "gargantuan.core/classes/FileLink.hpp"
#include "gargantuan.core/classes/Script.hpp"
#include "gargantuan.core/datatypes/Instance.hpp"
#include "gargantuan.core/render/Renderer.hpp"
#include "gargantuan.core/services/UserInputService.hpp"
#include "gargantuan.core/services/Workspace.hpp"
#include "gargantuan.runtime/ScriptEngine.hpp"

#include <SDL3/SDL.h>
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

		auto descendantAdded = [this](Instance::Pointer inst) {
			if (inst->IsClass<gargantuan::Script>()) {
				auto script = std::static_pointer_cast<gargantuan::Script>(inst);
				this->Script->ScriptQueue.insert(script);
				inst->Destroying->Once([ScriptEngine = this->Script, script](std::monostate _) {
					if (ScriptEngine->ScriptQueue.contains(script)) ScriptEngine->ScriptQueue.erase(script);
				});
			}

			if (inst->IsClass<gargantuan::FileLink>()) {
				auto link = std::static_pointer_cast<gargantuan::FileLink>(inst);
				auto relativePath = link->Path;
				auto absolutePath = std::filesystem::absolute(this->DataModel->Root / relativePath);
				LOG_INFO(
					App,
					"Got file link: %s %s %s",
					inst->GetFullName().c_str(),
					absolutePath.c_str(),
					relativePath.c_str()
				);
				link->Synchronize(absolutePath);
			}
		};

		auto descendantRemoved = [this](Instance::Pointer inst) {
			if (inst->IsClass<gargantuan::Script>()) {
				auto script = std::static_pointer_cast<gargantuan::Script>(inst);
				if (Script->ScriptQueue.contains(script)) Script->ScriptQueue.erase(script);
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

						Workspace->CurrentCamera->ViewportSize = Vector2(width, height);

						continue;
					}

					case SDL_EVENT_QUIT:
						LOG_INFO(App, "Stopping engine");
						ProcessService->Alive = false;
						return;
					}

					UserInputService->ProcessEvent(event);
					Workspace->CurrentCamera->OnEvent(event);
				}
			}

			{
				G_PROFILE("Simulation");
				RunService->PreSimulation->Fire(deltaTime);
				Workspace->CurrentCamera->Step(deltaTime);
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
					.Camera = Workspace->CurrentCamera,
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
