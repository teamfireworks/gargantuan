#pragma once

#include "gargantuan.runtime/ScriptEngine.hpp"
#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/classes/WorldRoot.hpp"
#include "gargantuan/render/Renderer.hpp"
#include "gargantuan/services/ProcessService.hpp"
#include "gargantuan/services/RunService.hpp"
#include "gargantuan/services/UserInputService.hpp"
#include "gargantuan/services/Workspace.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <glm/gtc/matrix_transform.hpp>
#include <lua.h>
#include <memory>
#include <type_traits>

namespace gargantuan {
	// TODO: Move much of this into DataModel
	struct Engine {
	  public:
		std::shared_ptr<DataModel> DataModel;
		BaseRenderer *Renderer;
		ScriptEngine *Script;

		std::shared_ptr<Workspace> Workspace;
		std::shared_ptr<WorldRoot> WorldRoot;
		std::shared_ptr<RunService> RunService;
		std::shared_ptr<ProcessService> ProcessService;
		std::shared_ptr<UserInputService> UserInputService;

		bool IsRunning = true;

		Engine(std::shared_ptr<gargantuan::DataModel> game, BaseRenderer *renderer);
		~Engine() = delete;

		void Step();
		float GetDeltaTime();
		void ProcessEvent(SDL_Event event);
		void Destroy();

	  private:
		uint64_t CurrentTick = 0;
		uint64_t LastTick = 0;

		template <typename T>
			requires std::is_base_of_v<Instance, T>
		std::shared_ptr<T> GetService() {
			return std::dynamic_pointer_cast<T>(this->DataModel->GetService(T::CLASS_DEFINITION.ClassName));
		}
	};

} // namespace gargantuan
