#pragma once

#include "gargantuan/classes/generated/ServiceProvider.hpp"

namespace gargantuan {
	class ServiceProvider : public Instance {
		I_ServiceProvider;

		typedef std::unordered_map<std::string, InstanceClassDefinition> ServiceDefinitions;
		std::unordered_map<std::string, std::shared_ptr<Instance>> Services;

		virtual const ServiceDefinitions &GetServiceDefinitions() const = 0;
	};
}
