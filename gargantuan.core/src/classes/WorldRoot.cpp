#include "gargantuan.core/classes/WorldRoot.hpp"
#include "gargantuan.core/classes/BasePart.hpp"
#include "gargantuan.core/reflection/InstanceClassRegistry.hpp"

#include <SDL3/SDL_log.h>
#include <memory>

namespace gargantuan {
	G_INSTANCE_ABSTRACT_IMPL(WorldRoot);

	WorldRoot::WorldRoot() {
		auto checkChildAdded = [this](Instance::Pointer instance) {
			if (instance->IsA("BasePart")) {
				std::shared_ptr<BasePart> part = std::static_pointer_cast<BasePart>(instance);
				this->Parts.push_back(part);
			}
		};

		for (const auto &instance : GetDescendants()) {
			checkChildAdded(instance);
		}
		ChildAdded->Connect(checkChildAdded);
	}
} // namespace gargantuan
