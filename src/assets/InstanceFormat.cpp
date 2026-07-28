#include "gargantuan/assets/InstanceFormat.hpp"
#include "gargantuan/ClassRegistry.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/datatypes/Enum.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/datatypes/UDim.hpp"
#include "gargantuan/datatypes/Vector2.hpp"

#include <cstring>
#include <nlohmann/json.hpp>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

// NOTE: InstanceFormat is based off Rojo's model format with a few exceptions:
// - All properties are assumed to be explicit for forward compatibility
// - Serializables are not restricted to just instances, ie. it can also be used
//   for configuration files
// - Because of this, removed Enum's explicit format to implement a new EnumItem
//   explicit format that also specifies the EnumType
// - Int32 and Int64 are merged into Int as those are irrelevant to Gargantuan

namespace gargantuan::InstanceFormat {
	using json = nlohmann::json;
	using Serializable =
		std::variant<bool, CFrame, Color3, double, EnumItem, float, glm::vec3, int, std::string_view, UDim, Vector2>;

	struct SerializationState {
		std::unordered_map<Instance::Pointer, json> InstanceMap;
	};

	using SerializedPair = std::pair<const char *, json>;
	std::optional<SerializedPair> TrySerializeValue(std::any unknown) {
		if (!unknown.has_value()) return std::nullopt;

		if (auto *value = std::any_cast<bool>(&unknown)) {
			return SerializedPair{"Bool", *value};
		} else if (auto *value = std::any_cast<CFrame>(&unknown)) {
			glm::vec3 position = value->Position;
			glm::mat3 rotation = value->Rotation;
			std::vector<float> components{
				position.x,
				position.y,
				position.z,
				rotation[0][0],
				rotation[0][1],
				rotation[0][2],
				rotation[1][0],
				rotation[1][1],
				rotation[1][2],
				rotation[2][0],
				rotation[2][1],
				rotation[2][2]
			};
			return SerializedPair{"CFrame", components};
		} else if (auto *value = std::any_cast<Color3>(&unknown)) {
			return SerializedPair{"Color3", {value->R, value->G, value->B}};
		} else if (auto *value = std::any_cast<double>(&unknown)) {
			return SerializedPair{"Double", *value};
		} else if (auto *value = std::any_cast<EnumItem>(&unknown)) {
			return SerializedPair{"EnumItem", {value->EnumType->Name, value->Name}};
		} else if (auto *value = std::any_cast<float>(&unknown)) {
			return SerializedPair{"Float", *value};
		} else if (auto *value = std::any_cast<glm::vec3>(&unknown)) {
			return SerializedPair{"Vector3", {value->x, value->y, value->z}};
		} else if (auto *value = std::any_cast<int>(&unknown)) {
			return SerializedPair{"Int", *value};
		} else if (auto *value = std::any_cast<std::string_view>(&unknown)) {
			// return json{"String", *value};
			return std::nullopt;
		} else if (auto *value = std::any_cast<UDim>(&unknown)) {
			return SerializedPair{"UDim", {value->Scale, value->Offset}};
		} else if (auto *value = std::any_cast<Vector2>(&unknown)) {
			return SerializedPair{"Vector2", {value->GetX(), value->GetY()}};
		} else {
			return std::nullopt;
		}
	}

	void SerializeProperties(Instance::ClassDefinition *definition, Instance::Pointer instance, json &properties) {
		for (auto &[key, prop] : definition->Properties) {
			if (key == "Parent" || !prop.Serializable || !prop.Read) continue;

			auto value = prop.Read(instance.get());
			if (auto serialized = TrySerializeValue(value); serialized.has_value()) {
				properties[key] = json::object({serialized.value()});
			}
		}

		if (definition->Superclass.has_value()) {
			auto *superclass = ClassRegistry::GetDefinitionByName(definition->Superclass.value());
			SerializeProperties(superclass, instance, properties);
		}
	}

	json SerializeInstance(Instance::Pointer instance, SerializationState &state) {
		if (state.InstanceMap.contains(instance)) {
			return state.InstanceMap.at(instance);
		}

		std::vector<json> children;
		children.reserve(instance->Children.size());
		for (auto &child : instance->Children) {
			children.emplace_back(SerializeInstance(child, state));
		}

		auto *definition = ClassRegistry::GetDefinition(instance.get());
		auto properties = json::object();
		SerializeProperties(definition, instance, properties);

		json serialized;
		serialized["ClassName"] = definition->Name;
		serialized["Children"] = children;
		serialized["Properties"] = properties;

		state.InstanceMap.emplace(instance, serialized);
		return serialized;
	}

	json SerializeJson(Instance::Pointer instance) {
		SerializationState state;
		auto serialized = SerializeInstance(instance, state);
		serialized["Version"] = 0;
		return serialized;
	};

	Instance::Pointer DeserializeJson() {
		return nullptr;
	};
}
