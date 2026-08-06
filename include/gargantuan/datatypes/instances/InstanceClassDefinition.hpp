#pragma once

#include "gargantuan/scripting/UserdataMethod.hpp"

#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace gargantuan {
	class Instance;
	template <typename Type> class InstanceProperty;

	struct InstanceClassDefinition {
	  public:
		std::string ClassName = "Instance";
		std::function<std::shared_ptr<Instance>()> Constructor;

		std::string_view Description = "(No description provided.)";
		std::function<void()> Register = []() {};
		std::optional<std::string_view> Superclass = "Instance";
		std::unordered_map<std::string_view, InstanceProperty<std::any>> Properties = {};
		std::unordered_map<std::string_view, UserdataMethod<Instance>> Methods = {};

		bool Flattened = false;
		std::unordered_map<std::string_view, const InstanceProperty<std::any> *> AllProperties = {};
		std::unordered_map<std::string_view, const UserdataMethod<Instance> *> AllMethods = {};
	};
}
