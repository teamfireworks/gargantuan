#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/classes/Instance.hpp"

#include <SDL3/SDL.h>

#include <cstddef>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace gargantuan::InstanceClassRegistry {
	std::unordered_map<std::type_index, InstanceClassDefinition> &GetDefinitionsMap() {
		static std::unordered_map<std::type_index, InstanceClassDefinition> definitions;
		return definitions;
	}

	namespace {
		std::unordered_map<std::string, std::type_index> &GetNameToTypeMap() {
			static std::unordered_map<std::string, std::type_index> index;
			return index;
		}

		bool NameIndexBuilt = false;

		void EnsureNameIndex() {
			if (NameIndexBuilt) return;

			auto &nameMap = GetNameToTypeMap();
			nameMap.clear();
			for (auto &[type, def] : GetDefinitionsMap()) {
				if (!def.ClassName.empty()) {
					nameMap.emplace(def.ClassName, type);
				}
			}
			NameIndexBuilt = true;
		}

		void Flatten(InstanceClassDefinition *definition) {
			if (!definition || definition->Flattened) return;

			// Guard against recursive cyclic flattening
			definition->Flattened = true;

			definition->AllProperties.clear();
			definition->AllMethods.clear();

			// 1. Recurse up into Superclass FIRST
			if (definition->Superclass.has_value()) {
				const std::string superName = std::string(definition->Superclass.value());

				EnsureNameIndex();
				auto &nameMap = GetNameToTypeMap();
				auto it = nameMap.find(superName);

				if (it != nameMap.end()) {
					InstanceClassDefinition *superDef = GetDefinitionByType(it->second);
					if (superDef && superDef != definition) {
						if (!superDef->Flattened) {
							Flatten(superDef);
						}

						for (auto &[name, propPtr] : superDef->AllProperties) {
							definition->AllProperties[name] = propPtr;
						}
						for (auto &[name, methodPtr] : superDef->AllMethods) {
							definition->AllMethods[name] = methodPtr;
						}

						definition->InheritedClasses.insert(
							superDef->InheritedClasses.begin(), superDef->InheritedClasses.end()
						);
						definition->InheritedClasses.emplace(superName);
					}
				}
			}

			for (auto &entry : definition->Properties) {
				definition->AllProperties[entry.first] = &entry.second;
			}

			for (auto &entry : definition->Methods) {
				definition->AllMethods[entry.first] = &entry.second;
			}
		}
	}

	void InvalidateCaches() {
		NameIndexBuilt = false;
		GetNameToTypeMap().clear();
		for (auto &entry : GetDefinitionsMap()) {
			entry.second.Flattened = false;
			entry.second.AllProperties.clear();
			entry.second.AllMethods.clear();
			entry.second.InheritedClasses.clear();
		}
	}

	InstanceClassDefinition *GetDefinitionByType(std::type_index type) {
		auto &map = GetDefinitionsMap();
		auto it = map.find(type);
		if (it == map.end()) return nullptr;

		InstanceClassDefinition *definition = &it->second;
		if (!definition->Flattened) {
			Flatten(definition);
		}

		return definition;
	}

	InstanceClassDefinition *GetDefinition(Instance *instance) {
		if (!instance) return nullptr;

		// Always resolve dynamically via typeid to guard against dangling/stale pointers
		InstanceClassDefinition *def = GetDefinitionByType(std::type_index(typeid(*instance)));
		if (def) {
			instance->CachedDefinition = def;
		}
		return def;
	}

	InstanceClassDefinition *GetDefinitionByName(std::string name) {
		EnsureNameIndex();
		auto &nameMap = GetNameToTypeMap();
		auto it = nameMap.find(std::string(name));
		if (it == nameMap.end()) return nullptr;

		return GetDefinitionByType(it->second);
	}

	std::vector<std::string> GetClassNames() {
		auto &map = GetDefinitionsMap();
		std::vector<std::string> result;
		result.reserve(map.size());
		for (auto &definition : map) {
			result.emplace_back(definition.second.ClassName);
		}
		return result;
	}
}
