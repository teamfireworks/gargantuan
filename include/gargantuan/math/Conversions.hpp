#pragma once

#include <box3d/box3d.h>
#include <glm/glm.hpp>

namespace gargantuan {
	b3Vec3 AsB3Vec3(const glm::vec3 &vector);
	b3Quat AsB3Quat(const glm::quat &quaternion);
	glm::vec3 AsGlmVec3(const b3Vec3 &vector);
	glm::quat AsGlmVec3(const b3Quat &quaternion);
	float AsB3Limit(float value); // b3 doesn't understand the lua "inf" and crashes
}
