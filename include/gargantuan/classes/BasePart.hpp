#pragma once

#include "gargantuan/classes/generated/BasePart.hpp"
#include "gargantuan/render/GpuMesh.hpp"

#include <box3d/box3d.h>
#include <glm/glm.hpp>

namespace gargantuan {
	class BasePart : public Instance {
		I_BasePart;

		glm::vec3 AccumulatedImpulse = {0.0f, 0.0f, 0.0f};
		virtual void CreateBodyShape(b3BodyId bodyId, b3ShapeDef &bodyShape) = 0;

		glm::mat4 GetModelMatrix();
		virtual std::unique_ptr<GpuMesh> &GetMesh() const = 0;
	};
} // namespace gargantuan
