#pragma once

#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/classes/generated/WorldRoot.hpp"

#include <box3d/box3d.h>
#include <box3d/id.h>
#include <memory>
#include <vector>

namespace gargantuan {
	class WorldRoot : public Instance {
		I_WorldRoot;

		b3WorldId World;

		std::vector<std::shared_ptr<BasePart>> Parts;
		std::unordered_map<BasePart *, b3BodyId> PartBodies;
		void KillWorld();

		void StepPhys(float deltaTime);

		WorldRoot();
	};
}
