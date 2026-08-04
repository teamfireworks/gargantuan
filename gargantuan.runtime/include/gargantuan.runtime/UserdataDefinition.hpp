#pragma once

#include "gargantuan.runtime/UserdataMethod.hpp"
#include "gargantuan.runtime/UserdataProperty.hpp"
#include "gargantuan.runtime/UserdataTag.hpp"
#include <string_view>
#include <unordered_map>
namespace gargantuan {
	template <typename Self> struct UserdataDefinition {
		UserdataTag Tag = UserdataTag::Invalid;
		std::string_view Type = "nil";
		std::unordered_map<std::string_view, UserdataProperty<Self>> Properties = {};
		std::unordered_map<std::string_view, UserdataMethod<Self>> Methods = {};
	};
}
