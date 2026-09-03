#include "gargantuan/math/Conversions.hpp"

#include <algorithm>
#include <cfloat>
#include <glm/gtc/quaternion.hpp>

namespace gargantuan {
	b3Vec3 AsB3Vec3(const glm::vec3 &vector) {
		return b3Vec3{vector.x, vector.y, vector.z};
	}

	b3Quat AsB3Quat(const glm::quat &quaternion) {
		return b3NormalizeQuat(b3Quat{{quaternion.x, quaternion.y, quaternion.z}, quaternion.w});
	}

	glm::vec3 AsGlmVec3(const b3Vec3 &vector) {
		return glm::vec3(vector.x, vector.y, vector.z);
	}

	glm::quat AsGlmVec3(const b3Quat &quaternion) {
		return glm::quat(quaternion.s, quaternion.v.x, quaternion.v.y, quaternion.v.z);
	}

	float AsB3Limit(float value) {
		if (std::isnan(value)) return 0.0f;
		return std::clamp(value, 0.0f, FLT_MAX);
	}
}
