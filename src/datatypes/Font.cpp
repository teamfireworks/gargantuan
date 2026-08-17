#include "gargantuan/datatypes/Font.hpp"
#include "gargantuan/scripting/Userdata.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

namespace gargantuan {
	G_USERDATA_IMPL(Font, .Tag = UserdataTag::Font, .Type = "Font");

	Font Font::fromName(std::string name) {
		Font self;
		self.Family = name;
		return self;
	}
}
