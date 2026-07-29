#include "gargantuan/Engine.hpp"
#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/render/MeshProvider.hpp"
#include "gargantuan/render/RenderProvider.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"
#include "gargantuan/services/Workspace.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <cstdlib>
#include <cstring>
#include <fwd.hpp>
#include <glm/glm.hpp>
#include <lua.h>
#include <luacode.h>
#include <lualib.h>
#include <memory>
#include <stdexcept>

namespace gargantuan {
	Engine::Engine() {
		this->Gpu = SDL_CreateGPUDevice(
			SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_METALLIB | SDL_GPU_SHADERFORMAT_MSL, true, nullptr
		);
		if (!Gpu) {
			throw std::runtime_error("Failed to instantiate GPU");
		}

		this->Window = SDL_CreateWindow(
			"Gargantuan",
			ViewportSize.x,
			ViewportSize.y,
			SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_HIGH_PIXEL_DENSITY
		);
		if (!Window) {
			throw std::runtime_error("Failed to instantiate window");
		}

		this->RenderProvider = new class RenderProvider(Window, Gpu);
		this->ScriptEngine = new class ScriptEngine();

		DataModel = std::make_shared<gargantuan::DataModel>();
		DataModel->Name = "Welcome To Hell";

		auto workspace = this->DataModel->GetService("Workspace");
		this->Workspace = std::dynamic_pointer_cast<gargantuan::Workspace>(workspace);

		auto runService = this->DataModel->GetService("RunService");
		this->RunService = std::dynamic_pointer_cast<gargantuan::RunService>(runService);

		auto uis = this->DataModel->GetService("UserInputService");
		this->UserInputService = std::dynamic_pointer_cast<gargantuan::UserInputService>(uis);

		StackValue<Instance::Pointer>::Push(ScriptEngine->L, this->DataModel);
		lua_pushvalue(ScriptEngine->L, -1);
		lua_setglobal(ScriptEngine->L, "game");
	}

	Engine::~Engine() {
		SDL_Log("destroying window");
		SDL_ReleaseWindowFromGPUDevice(Gpu, Window);
		SDL_DestroyWindow(Window);

		SDL_Log("destroying mesh provider");
		MeshProvider::Destroy(Gpu);

		RenderProvider->Destroy();

		SDL_Log("destroying gpu %s", Gpu ? "exists" : "not exist");
		SDL_DestroyGPUDevice(Gpu);
		Gpu = nullptr;
		SDL_Log("done destroying gpu");
	}

	void Engine::ProcessEvent(SDL_Event event) {
		switch (event.type) {
		case SDL_EVENT_QUIT:
			SDL_Log("Stopping");
			IsRunning = false;
			return;
		}
	}

	void Engine::Step() {
		if (!IsRunning) {
			return;
		}

		CurrentTick = SDL_GetTicks();
		if (LastTick == 0) {
			LastTick = CurrentTick;
		}
		float deltaTime = GetDeltaTime();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				IsRunning = false;
				return;
			}
			UserInputService->ProcessEvent(event);
			Workspace->CurrentCamera->OnEvent(Window, event);
		}

		RunService->PreSimulation->Fire(deltaTime);
		Workspace->CurrentCamera->Step(deltaTime);
		RunService->PostSimulation->Fire(deltaTime);

		RunService->PreRender->Fire(deltaTime);
		MeshProvider::UploadToGpu(Gpu);
		RenderProvider->Draw({
			.WorldRoot = std::static_pointer_cast<WorldRoot>(Workspace),
			.Camera = Workspace->CurrentCamera,
		});

		ScriptEngine->Step();

		LastTick = CurrentTick;
	}
} // namespace gargantuan
