#pragma once

#include "gargantuan/classes/Attachment.hpp"
#include "gargantuan/datatypes/Instance.hpp"

namespace gargantuan {
	class Constraint : public Instance {
	  public:
		G_INSTANCE_DECL(Constraint);

		bool Active;
		Attachment Attachment0;
		Attachment Attachment1;
		bool Enabled;
	};
} // namespace gargantuan
