#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {
	G_INSTANCE_ABSTRACT_IMPL(
		BasePart,
		.Description = "Abstract class for workspace objects that physically interact.",
		.Properties =
			{
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
				},
				{"Touched", Property::fromMember<&BasePart::Touched>(true, false)},
				{"TouchEnded", Property::fromMember<&BasePart::TouchEnded>(true, false)},
			},
		.Methods = {{"ApplyImpulse", Method::fromMember<&BasePart::ApplyImpulse>()}}
	);

	void BasePart::ApplyImpulse(glm::vec3 force) {
		World->ApplyImpulse(*this, force);
	}

	glm::mat4 BasePart::GetModelMatrix() {
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), CFrame.Position);
		glm::mat4 rotation = CFrame.Rotation;
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), Size);
		return translation * rotation * scale;
	}
} // namespace gargantuan
