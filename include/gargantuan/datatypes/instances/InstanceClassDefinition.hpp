#pragma once

#include "gargantuan/datatypes/instances/InstanceProperty.hpp"
#include "gargantuan/scripting/UserdataMethod.hpp"

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace gargantuan {
	class Instance;

	struct InstanceClassDefinition {
		std::string ClassName = "Instance";
		std::shared_ptr<Instance> (*Constructor)();
		std::string_view Description = "(No description provided.)";
		std::optional<std::string> Superclass = "Instance";
		std::unordered_map<std::string_view, InstanceProperty> Properties{};
		std::unordered_map<std::string_view, UserdataMethod<Instance>> Methods{};

		bool Flattened = false;
		std::unordered_set<std::string_view> InheritedClasses{};
		std::unordered_map<std::string_view, const InstanceProperty *> AllProperties{};
		std::unordered_map<std::string_view, const UserdataMethod<Instance> *> AllMethods{};
	};
}
