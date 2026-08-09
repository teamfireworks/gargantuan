#include "gargantuan/classes/ServiceProvider.hpp"

#include <SDL3/SDL.h>
#include <optional>

namespace gargantuan {
	std::optional<std::shared_ptr<Instance>> ServiceProvider::FindService(std::string name) {
		auto it = Services.find(std::string(name));
		if (it != Services.end()) {
			return it->second;
		}
		return nullptr;
	}

	std::shared_ptr<Instance> ServiceProvider::GetService(std::string nameView) {
		auto name = std::string(nameView);
		auto it = Services.find(name);
		if (it == Services.end()) {
			const ServiceDefinitions &constructors = GetServiceDefinitions();
			if (auto it = constructors.find(name); it != constructors.end()) {
				auto &definition = it->second;

				if (auto existing = FindFirstChildOfClass(definition.ClassName, std::nullopt)) {
					Services.emplace(name, existing);
					return existing;
				}

				auto service = definition.Constructor();
				service->SetParent(this->shared_from_this());
				Services.emplace(name, service);
				return service;
			} else {
				throw std::runtime_error(std::format("Unknown service named '{}'", name));
			}
		}
		return it->second;
	}
}
