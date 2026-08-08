#pragma once

#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/reflection/Enums.hpp"

namespace gargantuan {
	G_ENUM(StartCorner, TopLeft, TopRight, BottomLeft, BottomRight);

	class UICorner : public Instance {
	  public:
		G_INSTANCE_DECL(UICorner);

		Vector2 AbsoluteCellCount{0, 0};
		Vector2 AbsoluteCellSize{0, 0};
		// UDim2 CellPadding{0.0f, 0, 0.0f, 0};
		// UDim2 CellSize{0.0f, 64, 0.0f, 64};
		int FillDirectionMaxCells{0};
		Enums::StartCorner StartCorner{Enums::StartCorner::TopLeft};
	};
}
