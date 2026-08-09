#pragma once

#include "gargantuan/classes/generated/BasePart.hpp"
#include "gargantuan/render/GpuMesh.hpp"

#include <glm/glm.hpp>

namespace gargantuan {
	class BasePart : public Instance {
		I_BasePart;

		glm::mat4 GetModelMatrix();
		virtual std::unique_ptr<GpuMesh> &GetMesh() const = 0;
	};
} // namespace gargantuan
