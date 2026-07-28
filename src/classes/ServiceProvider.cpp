#include "gargantuan/classes/ServiceProvider.hpp"
#include <SDL3/SDL_log.h>
#include <stdexcept>
#include <string_view>

namespace gargantuan {
	const ServiceProvider::ClassDefinition ServiceProvider::DEFINITION = {
		.Name = "ServiceProvider",
		.Superclass = "Instance",
		.Methods = {
			{"FindService", Method::Wrap<&ServiceProvider::FindService>()},
			{"GetService", Method::Wrap<&ServiceProvider::GetService>()},
		}
	};

	Instance::Pointer ServiceProvider::FindService(std::string_view name) {
		auto it = Services.find(std::string(name));
		if (it != Services.end()) {
			return it->second;
		}
		return nullptr;
	}

	Instance::Pointer ServiceProvider::GetService(std::string_view nameView) {
		auto name = std::string(nameView);
		auto it = Services.find(name);
		if (it == Services.end()) {
			const ServiceConstructors &constructors = GetServiceConstructors();
			if (auto constructor = constructors.find(name); constructor != constructors.end()) {
				if (auto existing = FindFirstChild(name)) {
					Services.emplace(name, existing);
					return existing;
				}

				if (!constructor->second) {
					throw std::runtime_error("Missing constructor for service " + std::string(name));
				}
				auto service = constructor->second();
				// FIXME: instances should auto set names but im lazy
				service->SetParent(this->shared_from_this());
				Services.emplace(name, service);
				return service;
			} else {
				throw std::runtime_error("Unknown service");
			}
		}
		return it->second;
	}
} // namespace gargantuan
