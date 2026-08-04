#pragma once

#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

namespace gargantuan {
	typedef enum : int {
		File,
		Directory,
		Unknown,
	} FileType;

	typedef enum : int {
		// Open for reading, fails if missing.
		// Analogous to "r"
		Read,
		// Open for writing, creates if missing, overrides if it existed.
		// Analogous to "w"
		Write,
		// Open for writing, fails if it exists
		// Analogous to "x"
		Exclusive,
		// Open for appending, creates if missing
		// Analogous to "a"
		Append,
		// Open for reading and writing, fails if missing
		// Analogous to "r+"
		ReadWrite,
		// Open for reading and writing, creates if missing, overrides if it
		// existed. Analogous to "w+"
		Update,
		// Open for reading and appending, creates if missing.
		// Analogous to "a+"
		AppendRead
	} FileOpen;

	struct FileMetadata {
		FileType Type;
		unsigned int Size;
	};

	struct FileHandle {
		virtual ~FileHandle() = default;
		virtual size_t Read(void *buffer, std::size_t bytesToRead) = 0;
		virtual size_t Write(const void *buffer, std::size_t bytesToWrite) = 0;
		virtual size_t Size() = 0;
		virtual void Close() = 0;
	};

	struct DirectoryEntry {
		std::string Name;
		std::filesystem::path Path;
		FileType Type;
	};

	class BaseFilesystem {
	  public:
		BaseFilesystem(std::filesystem::path root) : Root(root) {};
		virtual ~BaseFilesystem() = default;

		std::filesystem::path Root;

		[[nodiscard]] virtual FileMetadata Metadata(const std::filesystem::path &path) const = 0;
		[[nodiscard]] virtual bool Exists(const std::filesystem::path &path) const = 0;
		[[nodiscard]] virtual std::unique_ptr<FileHandle>
		Open(const std::filesystem::path &path, const FileOpen &mode = FileOpen::Read) = 0;
		virtual void CreateDirectory(const std::filesystem::path &path) = 0;
		virtual void Remove(const std::filesystem::path &path) = 0;

		[[nodiscard]] virtual std::vector<DirectoryEntry> GetChildren(const std::filesystem::path &path) = 0;
		[[nodiscard]] virtual std::vector<DirectoryEntry> GetDescendants(const std::filesystem::path &path) = 0;

		[[nodiscard]] virtual FileType Type(const std::filesystem::path &path) const;
		virtual void Copy(const std::filesystem::path &source, const std::filesystem::path &destination);
		virtual void Move(const std::filesystem::path &source, const std::filesystem::path &destination);
		virtual std::string ReadFileToString(const std::filesystem::path &path);
		virtual void WriteStringToFile(const std::filesystem::path &path, std::string contents);
		virtual std::stringstream ReadFileToStringStream(const std::filesystem::path &path);
		virtual void WriteStringStreamToFile(const std::filesystem::path &path, std::ostringstream contents);
	};
}
