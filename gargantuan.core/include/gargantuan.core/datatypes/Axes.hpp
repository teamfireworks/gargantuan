#pragma once

#include "gargantuan.runtime/Userdata.hpp"
#include "gargantuan/reflection/Enums.hpp"

namespace gargantuan {
	G_ENUM(NormalId, Right, Top, Back, Left, Bottom, Front);
	G_USERDATA_DECL(
		Axes,

		bool Top = false;
		bool Bottom = false;
		bool Left = false;
		bool Right = false;
		bool Front = false;
		bool Back = false;

		void SetNormal(const Enums::NormalId &normal);
	);
}
