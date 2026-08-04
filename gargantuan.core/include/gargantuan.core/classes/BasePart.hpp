#pragma once

#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/render/GpuMesh.hpp"

#include <glm/glm.hpp>

namespace gargantuan {
	class BasePart : public Instance {
	  public:
		G_INSTANCE_DECL(BasePart);

		bool Anchored = false;
		bool CanCollide = true;
		bool CastShadow = true;
		CFrame CFrame;
		Color3 Color;
		glm::vec3 Size = glm::vec3(2, 1, 4);
		float Transparency = 0.0f;

		glm::mat4 GetModelMatrix();
		virtual std::unique_ptr<GpuMesh> &GetMesh() const = 0;
	};
} // namespace gargantuan
