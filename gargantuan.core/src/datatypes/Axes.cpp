#include "gargantuan.core/datatypes/Axes.hpp"
#include "gargantuan.runtime/Userdata.hpp"
#include "gargantuan.runtime/UserdataTag.hpp"

namespace gargantuan {
	G_USERDATA_IMPL(
		Axes,
		.Tag = UserdataTag::Axes,
		.Type = "Axes",
		.Properties = {
			{"Top", Property::fromReadonlyMember<&Axes::Top>()},
			{"Bottom", Property::fromReadonlyMember<&Axes::Bottom>()},
			{"Left", Property::fromReadonlyMember<&Axes::Left>()},
			{"Right", Property::fromReadonlyMember<&Axes::Right>()},
			{"Front", Property::fromReadonlyMember<&Axes::Front>()},
			{"Back", Property::fromReadonlyMember<&Axes::Back>()},
			{"X", Property::fromRead([](Axes *self) { return self->Left && self->Right; })},
			{"Y", Property::fromRead([](Axes *self) { return self->Top && self->Bottom; })},
			{"Z", Property::fromRead([](Axes *self) { return self->Front && self->Back; })},
		}
	)

	void Axes::SetNormal(const Enums::NormalId &normal) {
		switch (normal) {
		case Enums::NormalId::Top:
			Top = true;
			return;
		case Enums::NormalId::Bottom:
			Bottom = true;
			return;
		case Enums::NormalId::Left:
			Left = true;
			return;
		case Enums::NormalId::Right:
			Right = true;
			return;
		case Enums::NormalId::Front:
			Front = true;
			return;
		case Enums::NormalId::Back:
			Back = true;
			return;
		}
	}
}
