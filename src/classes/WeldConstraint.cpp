#include "gargantuan/classes/WeldConstraint.hpp"
#include <box3d/box3d.h>
#include <box3d/types.h>

namespace gargantuan {
	std::tuple<std::shared_ptr<BasePart>, std::shared_ptr<BasePart>> WeldConstraint::GetActiveParts() const {
		if (!Part0.has_value()) return {nullptr, nullptr};
		auto part0 = Part0.value();
		if (!part0->FindFirstAncestorWhichIsA("WorldRoot")) return {nullptr, nullptr};

		if (!Part1.has_value()) return {nullptr, nullptr};
		auto part1 = Part0.value();
		if (!part1->FindFirstAncestorWhichIsA("WorldRoot")) return {nullptr, nullptr};

		return {part0, part1};
	};

	b3JointId WeldConstraint::CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) {
		b3WeldJointDef jointDefinition = b3DefaultWeldJointDef();
		jointDefinition.base.bodyIdA = body0;
		jointDefinition.base.bodyIdB = body1;
		jointDefinition.base.collideConnected = true;
		return b3CreateWeldJoint(*world, &jointDefinition);
	}
}
