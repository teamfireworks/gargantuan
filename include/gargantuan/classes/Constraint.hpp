#pragma once

#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/classes/generated/Constraint.hpp"

#include <box3d/box3d.h>
#include <box3d/id.h>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <tuple>

namespace gargantuan {
	class Constraint : public Instance {
		I_Constraint;

		Constraint();

		b3JointId LeJoint = b3_nullJointId;

		std::function<void()> RequestRebuild;

		virtual std::tuple<std::shared_ptr<BasePart>, std::shared_ptr<BasePart>> GetActiveParts() const;
		virtual b3JointId CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) = 0;
		virtual void UpdateJoint() = 0;

		virtual void StepJoint(float deltaTime) {}

		virtual glm::quat GetFrameCorrection() const {
			return glm::identity<glm::quat>();
		}

	  protected:
		void BindJointProperties(std::initializer_list<std::string> propertyNames);
		void BindStructuralProperties(std::initializer_list<std::string> propertyNames);

		void UpdateJointFrames();
	};
}
