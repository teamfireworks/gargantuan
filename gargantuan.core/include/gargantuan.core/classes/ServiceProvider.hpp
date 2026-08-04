#pragma once

#include "gargantuan/datatypes/Instance.hpp"

#include <string_view>
#include <unordered_map>

namespace gargantuan {
	class ServiceProvider : public Instance {
	  public:
		G_INSTANCE_DECL(ServiceProvider);

		typedef std::unordered_map<std::string, InstanceClassDefinition> ServiceDefinitions;
		std::unordered_map<std::string, Instance::Pointer> Services;

		virtual Instance::Pointer FindService(std::string_view name);
		virtual Instance::Pointer GetService(std::string_view name);
		virtual const ServiceDefinitions &GetServiceDefinitions() const = 0;
	};
}
