#pragma once

#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/datatypes/UDim.hpp"

namespace gargantuan {
	class UICorner : public Instance {
	  public:
		G_INSTANCE_DECL(UICorner);

		UDim BottomLeftRadius{0.0f, 0};
		UDim BottomRightRadius{0.0f, 0};
		UDim TopLeftRadius{0.0f, 0};
		UDim TopRightRadius{0.0f, 0};
	};
}
