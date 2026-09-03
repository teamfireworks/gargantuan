#include "gargantuan/classes/SpringConstraint.hpp"
#include "gargantuan/math/Conversions.hpp"
#include <box3d/box3d.h>
#include <box3d/math_functions.h>
#include <box3d/types.h>
#include <cfloat>

namespace gargantuan {
	SpringConstraint::SpringConstraint() {
		BindJointProperties({
			"Damping",
			"FreeLength",
			"LimitsEnabled",
			"MaxLength",
			"MinLength",
			"Stiffness",
		});
	}

	b3JointId SpringConstraint::CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) {
		b3DistanceJointDef jointDefinition = b3DefaultDistanceJointDef();
		jointDefinition.base.bodyIdA = body0;
		jointDefinition.base.bodyIdB = body1;
		jointDefinition.base.collideConnected = true;
		return b3CreateDistanceJoint(*world, &jointDefinition);
	}

	void SpringConstraint::UpdateJoint() {
		UpdateJointFrames();

		// ok so now that the spring is based on mass of the bodies we gotta:
		// TODO: update spring joint on part mass changed (so Size/Anchored/Massless/Density props)
		float mA = b3Body_GetMass(b3Joint_GetBodyA(LeJoint));
		float mB = b3Body_GetMass(b3Joint_GetBodyB(LeJoint));
		float reduced = (mA > 0.0f && mB > 0.0f) ? (mA * mB) / (mA + mB) : std::max(mA, mB);

		float hertz = 0.0f, zeta = 0.0f;
		if (reduced > 0.0f && this->Stiffness > 0.0f) {
			float omega = std::sqrt(this->Stiffness / reduced);
			hertz = omega / (2.0f * B3_PI);
			zeta = this->Damping / (2.0f * std::sqrt(this->Stiffness * reduced));
		}

		b3DistanceJoint_EnableSpring(LeJoint, true);
		b3DistanceJoint_SetLength(LeJoint, this->FreeLength);
		b3DistanceJoint_SetSpringHertz(LeJoint, hertz);
		b3DistanceJoint_SetSpringDampingRatio(LeJoint, zeta);

		float maxForce = this->MaxForce > 0.0f ? this->MaxForce : FLT_MAX;
		b3DistanceJoint_SetSpringForceRange(LeJoint, -AsB3Limit(maxForce), AsB3Limit(maxForce));

		b3DistanceJoint_EnableLimit(LeJoint, this->LimitsEnabled);
		b3DistanceJoint_SetLengthRange(LeJoint, this->MinLength, this->MaxLength);
	}
}
