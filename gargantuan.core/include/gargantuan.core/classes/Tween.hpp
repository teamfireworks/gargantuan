#pragma once

#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/datatypes/TweenInfo.hpp"
#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/reflection/Enums.hpp"
#include <ext/vector_float3.hpp>
#include <lua.h>
#include <string>
#include <unordered_map>
#include <variant>

namespace gargantuan {
	G_ENUM(
		PlaybackState,

		Begin,
		Delayed,
		Playing,
		Paused,
		Completed,
		Cancelled
	)

	class Tween : public Instance {
	  public:
		G_INSTANCE_DECL(Tween);

		Instance::Pointer Instance;
		TweenInfo TweenInfo;

		using TweenableValue = std::variant<float, Color3, Vector2, glm::vec3>;
		using GoalPropertyMap = std::unordered_map<std::string, TweenableValue>;
		GoalPropertyMap GoalProperties;

		Enums::PlaybackState PlaybackState;

		void Play();
		void Cancel();
		void Pause();

		static int LPlay(lua_State *L, gargantuan::Instance *self);
		static int LStep(lua_State *L, gargantuan::Instance *self);

		G_SIGNAL(Completed, Enums::PlaybackState)
	  private:
		bool IsPlaying;
		double StartTime;
		double LerpStartTime;
		double EndTime;
		GoalPropertyMap InitialProperties;
	};
}
