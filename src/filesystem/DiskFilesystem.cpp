#include "gargantuan/filesystem/DiskFilesystem.hpp"
#include "gargantuan/filesystem/BaseFilesystem.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_iostream.h>
#include <filesystem>
#include <memory>
#include <stdexcept>

namespace gargantuan {
	struct DiskFileHandle final : public FileHandle {
		bool Closed = false;
		DiskFileHandle(SDL_IOStream *stream) : Stream(stream) {};
		~DiskFileHandle() {
			Close();
		};

		SDL_IOStream *Stream = nullptr;

		size_t Read(void *buffer, std::size_t bytesToRead) override {
			return SDL_ReadIO(Stream, buffer, bytesToRead);
		};

		size_t Write(const void *buffer, std::size_t bytesToWrite) override {
			return SDL_WriteIO(Stream, buffer, bytesToWrite);
		};

		size_t Size() override {
			return SDL_GetIOSize(Stream);
		};

		void Close() override {
			if (Closed) return;
			Closed = true;
			SDL_CloseIO(Stream);
		};
	};

	FileType MapSDLPathType(const SDL_PathType &type) {
		switch (type) {
		case SDL_PATHTYPE_FILE:
			return FileType::File;
		case SDL_PATHTYPE_DIRECTORY:
			return FileType::Directory;
		default:
			return FileType::Unknown;
		}
	}

	const char *MapFileOpen(const FileOpen &mode) {
		switch (mode) {
		case FileOpen::Read:
			return "r";
		case FileOpen::Write:
			return "w";
		case FileOpen::Exclusive:
			return "x";
		case FileOpen::Append:
			return "a";
		case FileOpen::ReadWrite:
			return "r+";
		case FileOpen::Update:
			return "w+";
		case FileOpen::AppendRead:
			return "a+";
		}
	}

	FileType MapDirectoryEntryType(const std::filesystem::directory_entry &entry) {
		if (entry.is_regular_file()) {
			return FileType::File;
		} else if (entry.is_directory()) {
			return FileType::Directory;
		} else {
			return FileType::Unknown;
		}
	}

	FileMetadata DiskFilesystem::Metadata(const std::filesystem::path &path) const {
		SDL_PathInfo info;
		if (!SDL_GetPathInfo(path.string().c_str(), &info)) throw std::runtime_error(SDL_GetError());
		return {
			.Type = MapSDLPathType(info.type),
			.Size = static_cast<unsigned int>(info.size),
		};
	}

	bool DiskFilesystem::Exists(const std::filesystem::path &path) const {
		return std::filesystem::exists(path);
	}

	std::unique_ptr<FileHandle> DiskFilesystem::Open(const std::filesystem::path &path, const FileOpen &mode) {
		auto stream = SDL_IOFromFile(path.string().c_str(), MapFileOpen(mode));
		if (!stream) throw SDL_GetError();
		return std::make_unique<DiskFileHandle>(stream);
	};

	void DiskFilesystem::CreateDirectory(const std::filesystem::path &path) {
		if (!std::filesystem::exists(path)) std::filesystem::create_directories(path);
	};

	void DiskFilesystem::Remove(const std::filesystem::path &path) {
		if (std::filesystem::exists(path)) std::filesystem::remove(path);
	};

	std::vector<DirectoryEntry> DiskFilesystem::GetChildren(const std::filesystem::path &path) {
		std::vector<DirectoryEntry> entries;
		for (const auto &entry : std::filesystem::directory_iterator(path)) {
			entries.push_back({
				.Name = std::move(entry.path().filename().string()),
				.Path = entry.path(),
				.Type = MapDirectoryEntryType(entry),
			});
		}
		return entries;
	};

	void CollectDescendants(std::vector<DirectoryEntry> &entries, const std::filesystem::path &path) {
		for (const auto &entry : std::filesystem::directory_iterator(path)) {
			entries.push_back({
				.Name = std::move(entry.path().filename().string()),
				.Path = entry.path(),
				.Type = MapDirectoryEntryType(entry),
			});

			if (entry.is_directory()) CollectDescendants(entries, entry.path());
		}
	};

	std::vector<DirectoryEntry> DiskFilesystem::GetDescendants(const std::filesystem::path &path) {
		std::vector<DirectoryEntry> entries;
		CollectDescendants(entries, path);
		return entries;
	};

	void DiskFilesystem::Copy(const std::filesystem::path &source, const std::filesystem::path &destination) {
		if (!std::filesystem::exists(source)) throw std::runtime_error("Source does not exist");
		if (std::filesystem::exists(destination)) throw std::runtime_error("Cannot copy to existing destination");
		if (std::filesystem::is_regular_file(source)) {
			SDL_CopyFile(source.string().c_str(), destination.string().c_str());
		}
	};
};
