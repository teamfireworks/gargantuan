#pragma once

#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/datatypes/Instance.hpp"

#include <box3d/box3d.h>
#include <box3d/id.h>
#include <ext/vector_float3.hpp>
#include <memory>
#include <vector>

namespace gargantuan {
	class WorldRoot : public Instance {
	  public:
		G_INSTANCE_DECL(WorldRoot);

		b3WorldId World;

		std::vector<std::shared_ptr<BasePart>> Parts;
		std::unordered_map<BasePart *, b3BodyId> PartBodies;
		void KillWorld();

		void StepPhys(float deltaTime);
		void ApplyImpulse(BasePart *part, glm::vec3 force);

		WorldRoot();
	};
} // namespace gargantuan
