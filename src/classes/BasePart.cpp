#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/datatypes/CFrame.hpp"

namespace gargantuan {
	glm::vec3 BasePart::GetPosition() const {
		return GetCFrame().Position;
	}

	void BasePart::SetPosition(glm::vec3 value) {
		SetCFrame(gargantuan::CFrame(value, GetCFrame().Rotation));
	}

	glm::mat4 BasePart::GetModelMatrix() {
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), CFrame.Position);
		glm::mat4 rotation = CFrame.Rotation;
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), Size);
		return translation * rotation * scale;
	}
}
