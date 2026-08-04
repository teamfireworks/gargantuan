#include "gargantuan.core/datatypes/UDim.hpp"
#include "gargantuan.runtime/Userdata.hpp"
#include "gargantuan.runtime/UserdataTag.hpp"
#include <common.hpp>
#include <sstream>

namespace gargantuan {
	G_USERDATA_IMPL(
		UDim,
		.Tag = UserdataTag::UDim,
		.Type = "UDim",
		.Properties =
			{
				{"Scale", Property::fromReadonlyMember<&UDim::Scale>()},
				{"Offset", Property::fromReadonlyMember<&UDim::Offset>()},
			},
		.Methods = {
			{"Lerp", Method::fromMember<&UDim::Lerp>()},
			{"__add", Method::fromMember<&UDim::Add>()},
			{"__sub", Method::fromMember<&UDim::Sub>()},
			{"__unm", Method::fromMember<&UDim::Unm>()},
			{"__eq", Method::fromMember<&UDim::Eq>()},
			{"__tostring", Method::fromMember<&UDim::Tostring>()},
		}
	);

	UDim::UDim(float scale, int offset) : Scale(scale), Offset(offset) {};

	UDim UDim::Lerp(const UDim &goal, float alpha) const {
		return UDim(Scale + (goal.Scale - Scale) * alpha, (int)glm::round(Offset + (goal.Offset - Offset) * alpha));
	}

	UDim UDim::Add(const UDim &other) const {
		return UDim(Scale + other.Scale, Offset + other.Offset);
	}

	UDim UDim::Sub(const UDim &other) const {
		return UDim(Scale - other.Scale, Offset - other.Offset);
	}

	UDim UDim::Unm() const {
		return UDim(-Scale, -Offset);
	}

	bool UDim::Eq(const UDim &other) const {
		return Scale == other.Scale && Offset == other.Offset;
	}

	std::string UDim::Tostring() const {
		std::ostringstream ss;
		ss << Scale << ", " << Offset;
		return ss.str();
	}
} // namespace gargantuan
