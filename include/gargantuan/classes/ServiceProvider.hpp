#pragma once

#include "gargantuan/datatypes/Instance.hpp"

#include <string_view>
#include <unordered_map>

namespace gargantuan {
	class ServiceProvider : public Instance {
	  public:
		G_INSTANCE_DECL(ServiceProvider);

		typedef std::unordered_map<std::string, InstanceClassDefinition> ServiceDefinitions;
		std::unordered_map<std::string, std::shared_ptr<Instance>> Services;

		virtual std::shared_ptr<Instance> FindService(std::string_view name);
		virtual std::shared_ptr<Instance> GetService(std::string_view name);
		virtual const ServiceDefinitions &GetServiceDefinitions() const = 0;
	};
}
