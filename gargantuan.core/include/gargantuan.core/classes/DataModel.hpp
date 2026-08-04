#pragma once

#include "gargantuan/classes/ServiceProvider.hpp"
// #include "gargantuan/filesystem/BaseFilesystem.hpp"

#include <filesystem>

namespace gargantuan {
	class DataModel : public ServiceProvider {
	  public:
		G_INSTANCE_DECL(DataModel);

		const ServiceDefinitions &GetServiceDefinitions() const override;

		std::filesystem::path Root;
		// BaseFilesystem *Filesystem = nullptr;
	};
} // namespace gargantuan
