#include "gargantuan/filesystem/BaseFilesystem.hpp"

#include <format>
#include <sstream>
#include <string>

namespace gargantuan {
	FileType BaseFilesystem::Type(const std::filesystem::path &path) const {
		return Metadata(path).Type;
	}

	void BaseFilesystem::Copy(const std::filesystem::path &source, const std::filesystem::path &destination) {
		if (!Exists(source)) throw std::format("File {} does not exist", source.string().c_str());
		if (Exists(destination)) throw std::format("Cannot copy to existing destination {}", destination.string().c_str());

		if (Type(source) == FileType::File) {
			if (Exists(destination) && Type(source) != FileType::File) {
				throw std::format("Cannot copy file {} to non-file {}", source.string().c_str(), destination.string().c_str());
			}

			auto sourceHandle = Open(source, FileOpen::Read);
			auto destinationHandle = Open(destination, FileOpen::Write);

			void *sourceContents;
			size_t sourceSize = sourceHandle->Size();
			sourceHandle->Read(sourceContents, sourceSize);
			destinationHandle->Write(sourceContents, sourceSize);

			sourceHandle->Close();
			destinationHandle->Close();
		} else {
			throw std::format("Unsupported source file type");
		}
	};

	void BaseFilesystem::Move(const std::filesystem::path &source, const std::filesystem::path &destination) {
		if (!Exists(source)) throw std::format("File {} does not exist", source.string().c_str());
		if (Exists(destination)) throw std::format("Cannot move to existing destination {}", destination.string().c_str());

		if (Type(source) == FileType::File) {
			if (Exists(destination) && Type(source) != FileType::File) {
				throw std::format("Cannot copy file {} to non-file {}", source.string().c_str(), destination.string().c_str());
			}

			auto sourceHandle = Open(source, FileOpen::Read);
			auto destinationHandle = Open(destination, FileOpen::Write);

			void *sourceContents;
			size_t sourceSize = sourceHandle->Size();
			sourceHandle->Read(sourceContents, sourceSize);
			destinationHandle->Write(sourceContents, sourceSize);

			sourceHandle->Close();
			destinationHandle->Close();
			Remove(source);
		} else {
			throw std::format("Unsupported source file type");
		}
	};

	std::string BaseFilesystem::ReadFileToString(const std::filesystem::path &path) {
		if (!Exists(path)) throw std::format("File {} does not exist", path.string().c_str());
		if (Type(path) != FileType::File) throw std::format("{} is not a file", path.string().c_str());

		auto handle = Open(path, FileOpen::Read);

		std::string result;
		result.resize(handle->Size());

		unsigned int bytesToWrite = handle->Size();
		auto bytesRead = handle->Read(result.data(), bytesToWrite);

		result.resize(bytesRead);
		handle->Close();

		return result;
	};

	void BaseFilesystem::WriteStringToFile(const std::filesystem::path &path, std::string contents) {
		if (!Exists(path)) throw std::format("File {} does not exist", path.string().c_str());
		if (Type(path) != FileType::File) throw std::format("{} is not a file", path.string().c_str());

		auto handle = Open(path, FileOpen::Write);
		handle->Write(contents.data(), contents.size());
		handle->Close();
	};

	std::stringstream BaseFilesystem::ReadFileToStringStream(const std::filesystem::path &path) {
		return std::stringstream(ReadFileToString(path));
	};

	void BaseFilesystem::WriteStringStreamToFile(const std::filesystem::path &path, std::ostringstream contents) {
		return WriteStringToFile(path, contents.str());
	};
}
