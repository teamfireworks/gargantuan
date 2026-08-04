#pragma once

#include "gargantuan/datatypes/Instance.hpp"
#include "nlohmann/json.hpp" // IWYU pragma: export

#include <format>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace gargantuan::InstanceSerialization {
	enum class InstanceFormat : int { Json, Binary };
	using json = nlohmann::json;

	struct DeserializationState {
		bool Ok = false;
		Instance::Pointer Instance;
		std::vector<std::string> Errors;

		std::vector<std::string_view> CurrentPath{"(TOP)"};
		std::string FormatCurrentPath();

		template <class... Args> void PushError(std::format_string<Args...> fmt, Args &&...args) {
			auto formatted = std::format(fmt, std::forward<Args>(args)...);
			Errors.push_back(formatted);
		}

		template <class... Args> std::nullopt_t ReturnError(std::format_string<Args...> fmt, Args &&...args) {
			auto formatted = std::format(fmt, std::forward<Args>(args)...);
			Errors.push_back(formatted);
			return std::nullopt;
		}
	};

	std::string Serialize(InstanceFormat format, Instance::Pointer &instance);
	DeserializationState Deserialize(InstanceFormat format, std::istream &input);
}
