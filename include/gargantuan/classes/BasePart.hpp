#pragma once

#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/classes/generated/BasePart.hpp"
#include "gargantuan/render/GpuMesh.hpp"

#include <glm/glm.hpp>

namespace gargantuan {
	class BasePart : public Instance {
		G_DECL_BASEPART;

		glm::mat4 GetModelMatrix();
		virtual std::unique_ptr<GpuMesh> &GetMesh() const = 0;
	};
} // namespace gargantuan
