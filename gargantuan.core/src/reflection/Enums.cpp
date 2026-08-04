#include "gargantuan.core/reflection/Enums.hpp"

namespace gargantuan::Enums {
	std::unordered_map<std::string_view, Enum::Pointer> &GetEnums() {
		static std::unordered_map<std::string_view, Enum::Pointer> enums;
		return enums;
	}

	void Register(Enum::Pointer self) {
		auto &enums = GetEnums();
		if (!enums.contains(self->Name)) {
			enums.emplace(self->Name, self);
		}
	}
}
