#include "gargantuan.core/classes/BasePart.hpp"
#include "gargantuan.core/datatypes/CFrame.hpp"
#include "gargantuan.core/reflection/InstanceClassRegistry.hpp"
#include "gargantuan.runtime/Userdata.hpp"

namespace gargantuan {
	G_INSTANCE_ABSTRACT_IMPL(
		BasePart,
		.Description = "Abstract class for workspace objects that physically interact.",
		.Properties = {
			{"Anchored", Property::fromMember<&BasePart::Anchored>(true, true).SetSerializable()},
			{"CanCollide", Property::fromMember<&BasePart::CanCollide>(true, true).SetSerializable()},
			{"CastShadow", Property::fromMember<&BasePart::CastShadow>(true, true).SetSerializable()},
			{"CFrame", Property::fromMember<&BasePart::CFrame>(true, true).SetSerializable()},
			{"Color", Property::fromMember<&BasePart::Color>(true, true).SetSerializable()},
			{"Size", Property::fromMember<&BasePart::Size>(true, true).SetSerializable()},
			{"Transparency", Property::fromMember<&BasePart::Transparency>(true, true).SetSerializable()},
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
	);

	glm::mat4 BasePart::GetModelMatrix() {
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), CFrame.Position);
		glm::mat4 rotation = CFrame.Rotation;
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), Size);
		return translation * rotation * scale;
	}
} // namespace gargantuan
