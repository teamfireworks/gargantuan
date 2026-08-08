#pragma once

#include "gargantuan/InstanceClassDefinition.hpp"

#include <string_view>
#include <typeindex>
#include <vector>

namespace gargantuan::InstanceClassRegistry {
	std::unordered_map<std::type_index, InstanceClassDefinition> &GetDefinitionsMap();

	void InvalidateCaches();

	template <typename T> void Register(InstanceClassDefinition definition) {
		auto &map = GetDefinitionsMap();
		map.emplace(std::type_index(typeid(T)), definition);
		InvalidateCaches();
	}

	InstanceClassDefinition *GetDefinitionByType(std::type_index type);

	template <typename T> InstanceClassDefinition *GetDefinition() {
		return GetDefinitionByType(std::type_index(typeid(T)));
	}

	InstanceClassDefinition *GetDefinition(Instance *instance);
	InstanceClassDefinition *GetDefinitionByName(std::string_view name);
	std::vector<std::string_view> GetClassNames();
}
