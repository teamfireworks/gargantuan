#pragma once

#include "gargantuan/classes/ServiceProvider.hpp"
#include "gargantuan/classes/generated/DataModel.hpp"
// #include "gargantuan/filesystem/BaseFilesystem.hpp"

#include <filesystem>

namespace gargantuan {
	class DataModel : public ServiceProvider {
		G_DECL_DATAMODEL;

		const ServiceDefinitions &GetServiceDefinitions() const override;

		std::filesystem::path Root;
		// BaseFilesystem *Filesystem = nullptr;
	};
} // namespace gargantuan
