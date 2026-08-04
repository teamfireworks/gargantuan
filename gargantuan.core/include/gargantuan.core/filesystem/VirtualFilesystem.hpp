#pragma once

#include "gargantuan/filesystem/BaseFilesystem.hpp"

#include <filesystem>

namespace gargantuan {
	class VirtualFilesystem final : public BaseFilesystem {
	  public:
		[[nodiscard]] FileMetadata Metadata(const std::filesystem::path &path) const override;
		[[nodiscard]] bool Exists(const std::filesystem::path &path) const override;
		[[nodiscard]] FileHandle
		Open(const std::filesystem::path &path, const FileOpen &mode = FileOpen::Read) override;
		void CreateDirectory(const std::filesystem::path &path) override;
		void Remove(const std::filesystem::path &path) override;

		[[nodiscard]] std::vector<DirectoryEntry> GetChildren(const std::filesystem::path &path) override;
		[[nodiscard]] std::vector<DirectoryEntry> GetDescendants(const std::filesystem::path &path) override;

		void Copy(const std::filesystem::path &source, const std::filesystem::path &destination) override;
	};
}
