#include "gargantuan/assets/InstanceFormat.hpp"
#include "gargantuan/ClassRegistry.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/datatypes/Enum.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/datatypes/UDim.hpp"
#include "gargantuan/datatypes/Vector2.hpp"

#include <SDL3/SDL_log.h>
#include <cstring>
#include <memory>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <sstream>
#include <string>
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

	// Serialization

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
			return SerializedPair{"String", std::string(value->data(), value->size())};
		} else if (auto *value = std::any_cast<UDim>(&unknown)) {
			return SerializedPair{"UDim", {value->Scale, value->Offset}};
		} else if (auto *value = std::any_cast<Vector2>(&unknown)) {
			return SerializedPair{"Vector2", {value->GetX(), value->GetY()}};
		} else {
			return std::nullopt;
		}
	}

	void SerializeProperties(Instance::ClassDefinition *definition, Instance::Pointer instance, json &properties) {
		for (auto &[key, property] : definition->Properties) {
			if (key == "Parent" || !property.Serializable || !property.Read || !property.Write) continue;

			auto value = property.Read(instance.get());
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

		nlohmann::ordered_json serialized;
		serialized["Name"] = instance->Name;
		serialized["ClassName"] = definition->Name;
		serialized["Properties"] = properties;
		serialized["Children"] = children;

		state.InstanceMap.emplace(instance, serialized);
		return serialized;
	}

	json SerializeJson(Instance::Pointer instance) {
		SerializationState state;
		auto serialized = SerializeInstance(instance, state);
		serialized["Version"] = 0;
		return serialized;
	};

	// Deserialization
	// FIXME: Current path refers to the parent when it really should refer to
	// the current child

	std::string DeserializationState::FormatCurrentPath() {
		std::ostringstream stream;
		int segmentCount = CurrentPath.size();
		for (int idx = 0; idx < segmentCount; idx++) {
			auto segment = CurrentPath[idx];
			stream << segment;
			if (idx != segmentCount) stream << " -> ";
		}
		return stream.str();
	}

	std::optional<std::any> TryDeserializeProperty(json unknown, DeserializationState &state) {
		if (unknown.size() == 0) return state.ReturnError("Missing explicit property to infer from");
		if (unknown.size() > 1) return state.ReturnError("Got too many possible property types");

		if (unknown.contains("Bool")) {
			auto value = unknown["Bool"];
			if (!value.is_boolean()) return state.ReturnError("Expected a boolean");
			return value.get<bool>();
		} else if (unknown.contains("CFrame")) {
			auto value = unknown["CFrame"];
			if (!value.is_array()) return state.ReturnError("Expected CFrame to be an array");

			auto x = value[0];
			if (!x.is_number_float()) return state.ReturnError("Expected X component to be a float");

			auto y = value[1];
			if (!y.is_number_float()) return state.ReturnError("Expected Y component to be a float");

			auto z = value[2];
			if (!z.is_number_float()) return state.ReturnError("Expected Z component to be a float");

			auto r00 = value[3];
			if (!r00.is_number_float()) return state.ReturnError("Expected R00 component to be a float");

			auto r01 = value[4];
			if (!r01.is_number_float()) return state.ReturnError("Expected R01 component to be a float");

			auto r02 = value[5];
			if (!r02.is_number_float()) return state.ReturnError("Expected R02 component to be a float");

			auto r10 = value[6];
			if (!r10.is_number_float()) return state.ReturnError("Expected R10 component to be a float");

			auto r11 = value[7];
			if (!r11.is_number_float()) return state.ReturnError("Expected R11 component to be a float");

			auto r12 = value[8];
			if (!r12.is_number_float()) return state.ReturnError("Expected R12 component to be a float");

			auto r20 = value[9];
			if (!r20.is_number_float()) return state.ReturnError("Expected R20 component to be a float");

			auto r21 = value[10];
			if (!r21.is_number_float()) return state.ReturnError("Expected R21 component to be a float");

			auto r22 = value[11];
			if (!r22.is_number_float()) return state.ReturnError("Expected R22 component to be a float");

			return CFrame(
				x.get<float>(),
				y.get<float>(),
				z.get<float>(),
				r00.get<float>(),
				r01.get<float>(),
				r02.get<float>(),
				r10.get<float>(),
				r11.get<float>(),
				r12.get<float>(),
				r20.get<float>(),
				r21.get<float>(),
				r22.get<float>()
			);
		} else if (unknown.contains("Color3")) {
			auto value = unknown["Color3"];
			if (!value.is_array()) return state.ReturnError("Expected Color3 to be an array");

			auto r = value[0];
			if (!r.is_number_float()) return state.ReturnError("Expected red component to be a float");

			auto g = value[1];
			if (!g.is_number_float()) return state.ReturnError("Expected green component to be a float");

			auto b = value[2];
			if (!b.is_number_float()) return state.ReturnError("Expected blue component to be a float");

			return Color3(r, g, b);
		} else if (unknown.contains("Double")) {
			auto value = unknown["Double"];
			if (!value.is_number()) return state.ReturnError("Expected double");
			return value.get<double>();
		} else if (unknown.contains("EnumItem")) {
			auto value = unknown["EnumItem"];
			if (!value.is_array()) return state.ReturnError("Expected EnumItem to be an array");

			auto enumType = value[0];
			if (!enumType.is_string()) return state.ReturnError("Expected EnumType to be a string");

			auto enumName = value[1];
			if (!enumName.is_string()) return state.ReturnError("Expected EnumName to be a string");

			auto enumTypeString = enumType.get<std::string>();
			auto &enums = Enums::GetEnums();
			if (!enums.contains(enumTypeString)) return state.ReturnError("Unknown EnumType {}", enumTypeString);

			auto enumNameString = enumName.get<std::string>();
			for (auto &item : enums.at(enumTypeString)->Items) {
				if (item.Name == enumNameString) {
					return item;
				}
			}

			return state.ReturnError("Unknown EnumItem named {} of Enum {}", enumNameString, enumTypeString);
		} else if (unknown.contains("Float")) {
			auto value = unknown["Float"];
			if (!value.is_number_float()) return state.ReturnError("Expected float");
			return value.get<float>();
		} else if (unknown.contains("Vector3")) {
			auto value = unknown["Vector3"];
			if (!value.is_array()) return state.ReturnError("Expected Vector3 to be an array");

			auto x = value[0];
			if (!x.is_number_float()) return state.ReturnError("Expected X component to be a float");

			auto y = value[1];
			if (!y.is_number_float()) return state.ReturnError("Expected Y component to be a float");

			auto z = value[2];
			if (!z.is_number_float()) return state.ReturnError("Expected Z component to be a float");

			return glm::vec3(x.get<float>(), y.get<float>(), z.get<float>());
		} else if (unknown.contains("Int")) {
			auto value = unknown["Int"];
			if (!value.is_number_integer()) return state.ReturnError("Expected int");
			return value.get<int>();
		} else if (unknown.contains("String")) {
			auto value = unknown["String"];
			if (!value.is_string()) return state.ReturnError("Expected string");
			return value.get<std::string>();
		} else if (unknown.contains("UDim")) {
			auto value = unknown["UDim"];
			if (!value.is_array()) return state.ReturnError("Expected UDim to be an array");

			auto scale = value[0];
			if (!scale.is_number_float()) return state.ReturnError("Expected Scale component to be a float");

			auto offset = value[1];
			if (!offset.is_number_integer()) return state.ReturnError("Expected Offset component to be an integer");

			return UDim(scale.get<float>(), offset.get<int>());
		} else if (unknown.contains("Vector2")) {
			auto value = unknown["Vector2"];
			if (!value.is_array()) return state.ReturnError("Expected Vector2 to be an array");

			auto x = value[0];
			if (!x.is_number_float()) return state.ReturnError("Expected X component to be a float");

			auto y = value[1];
			if (!y.is_number_float()) return state.ReturnError("Expected Y component to be a float");

			return Vector2(x.get<float>(), y.get<float>());
		}

		return state.ReturnError("Unsupported property value: %s", unknown.dump());
	};

	std::optional<Instance::Pointer> TryDeserializeInstance(json contents, DeserializationState &state) {
		auto properties = contents["Properties"];
		if (!properties.is_object()) {
			state.PushError("Instance {} has an invalid Properties field", state.FormatCurrentPath());
			return std::nullopt;
		}

		auto children = contents["Children"];
		if (!children.is_array()) {
			state.PushError("Instance {} has an invalid Children field", state.FormatCurrentPath());
			return std::nullopt;
		}

		auto maybeClassName = contents["ClassName"];
		if (!maybeClassName.is_string()) {
			state.PushError("Instance {} has an invalid ClassName field", state.FormatCurrentPath());
			return std::nullopt;
		}

		auto className = maybeClassName.get<std::string>();
		auto definition = ClassRegistry::GetDefinitionByName(className);
		if (!definition) {
			state.PushError("Instance {} has unknown ClassName '{}'", state.FormatCurrentPath(), className);
			return std::nullopt;
		} else if (!definition->Constructor) {
			state.PushError("Cannot deserialize instance {} of class {}", state.FormatCurrentPath(), className);
			return std::nullopt;
		}

		auto instance = definition->Constructor();
		// temporary
		instance->Name = definition->Name;
		while (true) {
			for (auto &[key, property] : definition->Properties) {
				if (key == "Parent" || !property.Serializable || !property.Write || !properties.contains(key)) continue;

				auto value = properties[key];
				if (!value.is_object()) {
					state.PushError("Invalid value of property '{}' in {}", key, state.FormatCurrentPath());
					instance->Destroy();
					return std::nullopt;
				}

				auto maybeDeserialized = TryDeserializeProperty(value, state);
				if (!maybeDeserialized.has_value()) {
					state.PushError(
						"Failed to deserialize value of property '{}' in {}", key, state.FormatCurrentPath()
					);
					instance->Destroy();
					return std::nullopt;
				}

				auto deserialized = maybeDeserialized.value();

				try {
					property.Write(instance.get(), deserialized);
				} catch (const std::bad_any_cast &e) {
					instance->Destroy();
					return state.ReturnError(
						"Type mismatch on property '{}' in {}, got approximately {}",
						key,
						state.FormatCurrentPath(),
						typeid(deserialized).name()
					);
				} catch (const std::exception &e) {
					instance->Destroy();
					return state.ReturnError(
						"Failed to set value of property '{}' in {}: {}", key, state.FormatCurrentPath(), e.what()
					);
				} catch (...) {
					instance->Destroy();
					return state.ReturnError(
						"Unknown error setting property '{}' in {}", key, state.FormatCurrentPath()
					);
				}
			}

			if (definition->Superclass.has_value()) {
				definition = ClassRegistry::GetDefinitionByName(definition->Superclass.value());
			} else {
				break;
			}
		}

		state.CurrentPath.push_back(instance->Name);

		for (auto &child : children) {
			auto maybeChild = TryDeserializeInstance(child, state);
			if (maybeChild.has_value()) {
				maybeChild.value()->SetParent(instance);
			} else {
				state.PushError("Failed to deserialize child in {}", state.FormatCurrentPath());
				instance->Destroy();
				return std::nullopt;
			}
		}

		state.CurrentPath.pop_back();

		return instance;
	}

	DeserializationState DeserializeJson(json contents) {
		DeserializationState state;

		if (!contents.is_object()) {
			state.PushError("Expected a JSON object");
			return state;
		}

		if (contents["Version"] != 0) {
			state.PushError("Unsupported instance format version");
			return state;
		}

		auto maybeInstance = TryDeserializeInstance(contents, state);
		if (maybeInstance.has_value()) {
			state.Ok = true;
			state.Instance = maybeInstance.value();
		}

		return state;
	};
}
