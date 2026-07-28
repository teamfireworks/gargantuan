#pragma once

#include "gargantuan/datatypes/Instance.hpp"
#include "nlohmann/json.hpp" // IWYU pragma: export

#include <format>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace gargantuan::InstanceFormat {
	using json = nlohmann::json;
	json SerializeJson(Instance::Pointer);

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

	DeserializationState DeserializeJson(json contents);
}
