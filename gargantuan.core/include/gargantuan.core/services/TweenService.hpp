#pragma once

#include "gargantuan/classes/Tween.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/datatypes/TweenInfo.hpp"
#include <memory>

namespace gargantuan {
	class TweenService : public Instance {
	  public:
		G_INSTANCE_DECL(TweenService);

		Tween::Pointer Create(Instance::Pointer instance, TweenInfo tweenInfo, Tween::GoalPropertyMap goalProperties) {
			return nullptr;
			// auto tween = std::make_shared<Tween>();

			// do... stuff?

			// return tween;
		}
	};
}
