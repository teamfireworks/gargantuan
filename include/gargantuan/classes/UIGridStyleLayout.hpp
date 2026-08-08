#pragma once

#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/reflection/Enums.hpp"

namespace gargantuan {
	G_ENUM(FillDirection, Horizontal, Vertical);
	G_ENUM(SortOrder, Name, LayoutOrder);
	G_ENUM(HorizontalAlignment, Left, Center, Right);
	G_ENUM(VerticalAlignment, Top, Center, Bottom);

	class UIGridStyleLayout : public Instance {
	  public:
		G_INSTANCE_DECL(UIGridStyleLayout);

		Vector2 AbsoluteContentSize{0.0f, 0.0f};
		Enums::FillDirection FillDirection{Enums::FillDirection::Vertical};
		Enums::HorizontalAlignment HorizontalAlignment{Enums::HorizontalAlignment::Center};
		Enums::SortOrder SortOrder{Enums::SortOrder::LayoutOrder};
		Enums::VerticalAlignment VerticalAlignment{Enums::VerticalAlignment::Center};
	};
}
