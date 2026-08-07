#pragma once

#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/render/GpuMesh.hpp"

#include <glm/glm.hpp>
class BasePart;
class WorldRoot {
  public:
	void ApplyImpulse(BasePart *part, glm::vec3 force);
};

namespace gargantuan {
	class BasePart : public Instance {
	  public:
		G_INSTANCE_DECL(BasePart);

		WorldRoot *World = nullptr;

		bool Anchored = false;
		bool CanCollide = true;
		bool CastShadow = true;
		CFrame CFrame;
		Color3 Color;
		glm::vec3 Size = glm::vec3(2, 1, 4);
		float Transparency = 0.0f;

		G_SIGNAL(Touched, BasePart *);
		G_SIGNAL(TouchEnded, BasePart *);
		void ApplyImpulse(glm::vec3 force);

		glm::mat4 GetModelMatrix();
		virtual std::unique_ptr<GpuMesh> &GetMesh() const = 0;
	};
} // namespace gargantuan
