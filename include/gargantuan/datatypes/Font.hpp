#pragma once

#include "gargantuan/scripting/Userdata.hpp"

#include <string>

namespace gargantuan {
	namespace Enums {
		enum class FontWeight;
		enum class FontStyle;
	}

	typedef std::string Content;

	G_USERDATA_DECL(
		Font,

		Font(){};
		Font(Content fontContent) : Family(fontContent){};
		Font(Content fontContent, Enums::FontWeight weight) : Family(fontContent), Weight(weight){};
		Font(Content fontContent, Enums::FontWeight weight, Enums::FontStyle style) : Family(fontContent),
		Weight(weight),
		Style(style){};
		static Font fromName(std::string name);

		Content Family = "";
		Enums::FontWeight Weight;
		Enums::FontStyle Style;
	);
}
