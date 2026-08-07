#pragma once

#include "gargantuan/classes/Constraint.hpp"
#include "gargantuan/classes/Part.hpp"
#include "gargantuan/datatypes/Instance.hpp"

namespace gargantuan {
	class WeldConstraint : public Constraint {
	  public:
		G_INSTANCE_DECL(WeldConstraint);

		Part Part0;
		Part Part1;
	};
} // namespace gargantuan
