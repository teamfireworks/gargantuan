#pragma once

#include "gargantuan/classes/WorldRoot.hpp"
#include "gargantuan/services/generated/Workspace.hpp"

namespace gargantuan {
	class Workspace : public WorldRoot {
		I_Workspace;

		Workspace();
	};
}
