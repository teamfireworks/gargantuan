#pragma once

#include "gargantuan/datatypes/Instance.hpp"

#include <string_view>
#include <typeindex>
#include <vector>

#define G_INSTANCE_IMPL(instanceType, ...)                                                                             \
	const gargantuan::InstanceClassDefinition instanceType::CLASS_DEFINITION = {                                       \
		.ClassName = #instanceType,                                                                                    \
		.Constructor = []() -> std::shared_ptr<gargantuan::Instance> {                                                 \
			return std::make_shared<gargantuan::instanceType>();                                                       \
		},                                                                                                             \
		__VA_ARGS__                                                                                                    \
	};                                                                                                                 \
	namespace InstanceClassRegistry::__IGNORE__ {                                                                      \
		bool instanceType = []() {                                                                                     \
			InstanceClassRegistry::Register<gargantuan::instanceType>(instanceType::CLASS_DEFINITION);                 \
			return true;                                                                                               \
		}();                                                                                                           \
	};

#define G_INSTANCE_ABSTRACT_IMPL(instanceType, ...)                                                                    \
	const gargantuan::InstanceClassDefinition instanceType::CLASS_DEFINITION = {                                       \
		.ClassName = #instanceType, __VA_ARGS__                                                                        \
	};                                                                                                                 \
	namespace InstanceClassRegistry::__IGNORE__ {                                                                      \
		bool instanceType = []() {                                                                                     \
			InstanceClassRegistry::Register<gargantuan::instanceType>(instanceType::CLASS_DEFINITION);                 \
			return true;                                                                                               \
		}();                                                                                                           \
	};

namespace gargantuan::InstanceClassRegistry {
	std::unordered_map<std::type_index, InstanceClassDefinition> &GetDefinitionsMap();

	template <typename T> void Register(InstanceClassDefinition definition) {
		auto &map = GetDefinitionsMap();
		map.emplace(std::type_index(typeid(T)), definition);
	}

	template <typename T> InstanceClassDefinition *GetDefinition() {
		auto map = GetDefinitionsMap();
		auto it = map.find(std::type_index(typeid(T)));
		if (it != map.end()) {
			return &it->second;
		}
		return nullptr;
	}

	InstanceClassDefinition *GetDefinition(Instance *instance);
	InstanceClassDefinition *GetDefinitionByName(std::string_view name);
	std::vector<std::string_view> GetClassNames();
}
