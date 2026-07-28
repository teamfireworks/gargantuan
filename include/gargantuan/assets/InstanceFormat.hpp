#pragma once

#include "gargantuan/datatypes/Instance.hpp"
#include "nlohmann/json.hpp"

namespace gargantuan::InstanceFormat {
	using json = nlohmann::json;
	json SerializeJson(Instance::Pointer);
	Instance::Pointer DeserializeJson(json contents);
}
