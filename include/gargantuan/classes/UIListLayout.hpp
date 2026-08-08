#pragma once

#include "gargantuan/classes/GuiObject.hpp"
#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/classes/UIGridStyleLayout.hpp"
#include "gargantuan/reflection/Enums.hpp"

namespace gargantuan {
	G_ENUM(FlexAlignment, None, Fill, SpaceAround, SpaceBetween, SpaceEvenly);
	G_ENUM(ItemLineAlignment, Automatic, Start, Center, End, Stretch);

	class UIListLayout : public UIGridStyleLayout {
	  public:
		G_INSTANCE_DECL(UIGridStyleLayout);

		Enums::FlexAlignment HorizontalFlex{Enums::FlexAlignment::None};
		Enums::FlexAlignment VerticalFlex{Enums::FlexAlignment::None};
		Enums::ItemLineAlignment ItemLineAlignment{Enums::ItemLineAlignment::Automatic};
		UDim Padding{0.0f, 0};
		bool Wraps{false};
	};
}
