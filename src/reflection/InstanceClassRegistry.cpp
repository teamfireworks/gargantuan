#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/classes/Instance.hpp"

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

	namespace {
		std::unordered_map<std::string_view, InstanceClassDefinition *> &GetNameIndex() {
			static std::unordered_map<std::string_view, InstanceClassDefinition *> index;
			return index;
		}

		bool NameIndexBuilt = false;

		void EnsureNameIndex() {
			if (NameIndexBuilt) {
				return;
			}

			auto &index = GetNameIndex();
			index.clear();
			for (auto &entry : GetDefinitionsMap()) {
				index.emplace(entry.second.ClassName, &entry.second);
			}
			NameIndexBuilt = true;
		}

		void Flatten(InstanceClassDefinition *definition) {
			definition->AllProperties.clear();
			definition->AllMethods.clear();

			for (InstanceClassDefinition *current = definition; current;) {
				for (auto &[name, property] : current->Properties) {
					definition->AllProperties.emplace(name, &property);
				}

				for (auto &[name, method] : current->Methods) {
					definition->AllMethods.emplace(name, &method);
				}

				if (!current->Superclass.has_value()) break;
				definition->InheritedClasses.emplace(current->Superclass.value());

				InstanceClassDefinition *next = GetDefinitionByName(current->Superclass.value());
				if (next == current) break;
				current = next;
			}

			definition->Flattened = true;
		}
	}

	void InvalidateCaches() {
		NameIndexBuilt = false;
		GetNameIndex().clear();
		for (auto &entry : GetDefinitionsMap()) {
			entry.second.Flattened = false;
			entry.second.AllProperties.clear();
			entry.second.AllMethods.clear();
		}
	}

	InstanceClassDefinition *GetDefinitionByType(std::type_index type) {
		auto &map = GetDefinitionsMap();
		auto it = map.find(type);
		if (it == map.end()) {
			return nullptr;
		}

		InstanceClassDefinition *definition = &it->second;
		if (!definition->Flattened) {
			Flatten(definition);
		}

		return definition;
	}

	InstanceClassDefinition *GetDefinition(Instance *instance) {
		if (!instance) return nullptr;

		if (instance->CachedDefinition) {
			if (!instance->CachedDefinition->Flattened) {
				Flatten(instance->CachedDefinition);
			}
			return instance->CachedDefinition;
		}

		instance->CachedDefinition = GetDefinitionByType(std::type_index(typeid(*instance)));
		return instance->CachedDefinition;
	};

	InstanceClassDefinition *GetDefinitionByName(std::string_view name) {
		EnsureNameIndex();
		auto &index = GetNameIndex();
		auto it = index.find(name);
		if (it == index.end()) return nullptr;

		InstanceClassDefinition *definition = it->second;
		if (!definition->Flattened) Flatten(definition);

		return definition;
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
