#pragma once

#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/datatypes/Signal.hpp"

namespace gargantuan {
	class RunService : public Instance {
	  public:
		G_INSTANCE_DECL(RunService);

		G_SIGNAL(PreSimulation, double);
		G_SIGNAL(PostSimulation, double);
		G_SIGNAL(PreRender, double);
	};
}
