#pragma once

#include "gargantuan/classes/Camera.hpp"
#include "gargantuan/classes/WorldRoot.hpp"
#include <memory>

namespace gargantuan {

	class Workspace : public WorldRoot {
	  public:
		G_INSTANCE_DECL(Workspace);

		std::shared_ptr<Camera> CurrentCamera = std::make_shared<Camera>();
	};

} // namespace gargantuan
