#pragma once

#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Instance.hpp"

namespace gargantuan {
	class Attachment : public Instance {
	  public:
		G_INSTANCE_DECL(Attachment);

		CFrame CFrame;
	};
} // namespace gargantuan
