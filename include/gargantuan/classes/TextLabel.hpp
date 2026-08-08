#pragma once

#include "gargantuan/classes/GuiObject.hpp"
#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/datatypes/Font.hpp"
#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/reflection/Enums.hpp"
#include <string>

namespace gargantuan {
	G_ENUM(TextDirection, Auto, LeftToRight, RightToLeft);
	G_ENUM(TextXAlignment, Left, Center, Right);
	G_ENUM(TextYAlignment, Top, Center, Bottom);

	class TextLabel : public GuiObject {
	  public:
		G_INSTANCE_DECL(TextLabel);

		std::string ContentText;
		Font FontFace;
		float LineHeight{1.0f};
		std::string LocalizedText{};
		int MaxVisibleGraphemes{-1};
		bool RichText{false};
		std::string Text{};
		Vector2 TextBounds{0.0f, 0.0f};
		bool TextScaled{true};
		bool TextFits{false};
		int TextSize{12};
		float TextTransparency{0.0f};
		bool TextWrapped{true};
		Enums::TextXAlignment TextXAlignment{Enums::TextXAlignment::Center};
		Enums::TextYAlignment TextYAlignment{Enums::TextYAlignment::Center};
	};

}
