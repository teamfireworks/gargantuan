#include "gargantuan/services/Workspace.hpp"
#include "gargantuan/classes/Camera.hpp"

#include <memory>

namespace gargantuan {
	Workspace::Workspace() {
		CurrentCamera = std::make_shared<Camera>();
	}
}
