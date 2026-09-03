#include "gargantuan/classes/WeldConstraint.hpp"
#include <box3d/box3d.h>
#include <box3d/math_functions.h>
#include <box3d/types.h>

namespace gargantuan {
	WeldConstraint::WeldConstraint() {
		BindStructuralProperties({"Part0", "Part1"});
	}

	std::tuple<std::shared_ptr<BasePart>, std::shared_ptr<BasePart>> WeldConstraint::GetActiveParts() const {
		if (!Part0.has_value()) return {nullptr, nullptr};
		auto part0 = Part0.value();
		if (!part0->FindFirstAncestorWhichIsA("WorldRoot")) return {nullptr, nullptr};

		if (!Part1.has_value()) return {nullptr, nullptr};
		auto part1 = Part1.value();
		if (!part1->FindFirstAncestorWhichIsA("WorldRoot")) return {nullptr, nullptr};

		return {part0, part1};
	};

	b3JointId WeldConstraint::CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) {
		b3WeldJointDef jointDefinition = b3DefaultWeldJointDef();
		jointDefinition.base.bodyIdA = body0;
		jointDefinition.base.bodyIdB = body1;
		jointDefinition.base.collideConnected = false;
		b3WorldTransform transform0 = b3Body_GetTransform(body0);
		b3WorldTransform transform1 = b3Body_GetTransform(body1);
		jointDefinition.base.localFrameA = b3Transform_identity;
		jointDefinition.base.localFrameB = b3InvMulWorldTransforms(transform1, transform0);

		return b3CreateWeldJoint(*world, &jointDefinition);
	}

	void WeldConstraint::UpdateJoint() {}
}
