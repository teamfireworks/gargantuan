#pragma once

#include "gargantuan/classes/GuiBase.hpp"
#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/datatypes/Vector2.hpp"

namespace gargantuan {
	class GuiBase2d : public GuiBase {
	  public:
		G_INSTANCE_DECL(GuiBase2d);

		Vector2 AbsolutePosition{0.0f, 0.0f};
		float AbsoluteRotation{0.0f};
		Vector2 AbsoluteSize{0.0f, 0.0f};
	};
}
