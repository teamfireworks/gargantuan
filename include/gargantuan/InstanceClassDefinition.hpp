#pragma once

#include "gargantuan/InstanceProperty.hpp"
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
		std::string Description = "(No description provided.)";
		std::optional<std::string> Superclass = "Instance";
		std::unordered_map<std::string, InstanceProperty> Properties{};
		std::unordered_map<std::string, UserdataMethod<Instance>> Methods{};

		bool Flattened = false;
		std::unordered_set<std::string> InheritedClasses{};
		std::unordered_map<std::string, const InstanceProperty *> AllProperties{};
		std::unordered_map<std::string, const UserdataMethod<Instance> *> AllMethods{};
	};
}
