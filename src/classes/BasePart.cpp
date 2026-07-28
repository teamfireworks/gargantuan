#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {
	const BasePart::ClassDefinition BasePart::DEFINITION = {
		.Name = "BasePart",
		.Superclass = "Instance",
		.Properties = {
			{"Anchored", Property::fromSimple<&BasePart::Anchored>(true, true).SetSerializable()},
			{"CanCollide", Property::fromSimple<&BasePart::CanCollide>(true, true).SetSerializable()},
			{"CastShadow", Property::fromSimple<&BasePart::CastShadow>(true, true).SetSerializable()},
			{"CFrame", Property::fromSimple<&BasePart::CFrame>(true, true).SetSerializable()},
			{"Color", Property::fromSimple<&BasePart::Color>(true, true).SetSerializable()},
			{"Size", Property::fromSimple<&BasePart::Size>(true, true).SetSerializable()},
			{"Transparency", Property::fromSimple<&BasePart::Transparency>(true, true).SetSerializable()},
			{
				"Position",
				Property::fromReadWrite<glm::vec3>(
					[](Instance *self) { return self->Cast<BasePart>()->CFrame.Position; },
					[](Instance *self, glm::vec3 value) {
						auto part = self->Cast<BasePart>();
						part->CFrame = gargantuan::CFrame(value, part->CFrame.Rotation);
					}
				),
			}
		}
	};

	glm::mat4 BasePart::GetModelMatrix() {
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), CFrame.Position);
		glm::mat4 rotation = CFrame.Rotation;
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), Size);
		return translation * rotation * scale;
	}
} // namespace gargantuan
