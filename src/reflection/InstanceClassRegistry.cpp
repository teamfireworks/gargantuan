#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/datatypes/Instance.hpp"

#include <SDL3/SDL_log.h>
#include <cstddef>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace gargantuan::InstanceClassRegistry {
	std::unordered_map<std::type_index, InstanceClassDefinition> &GetDefinitionsMap() {
		static std::unordered_map<std::type_index, InstanceClassDefinition> definitions;
		return definitions;
	}

	InstanceClassDefinition *GetDefinition(Instance *instance) {
		if (!instance) return nullptr;
		auto &map = GetDefinitionsMap();
		auto it = map.find(std::type_index(typeid(*instance)));
		if (it != map.end()) {
			return &it->second;
		}
		return nullptr;
	};

	InstanceClassDefinition *GetDefinitionByName(std::string_view name) {
		auto &map = GetDefinitionsMap();
		for (auto &definition : map) {
			if (definition.second.ClassName == name) {
				return &definition.second;
			}
		}
		return nullptr;
	}

	std::vector<std::string_view> GetClassNames() {
		auto &map = GetDefinitionsMap();
		std::vector<std::string_view> result;
		result.reserve(map.size());
		for (auto &definition : map) {
			result.emplace_back(definition.second.ClassName);
		}
		return result;
	}
}
