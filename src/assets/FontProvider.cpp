#include "gargantuan/assets/FontProvider.hpp"
#include "gargantuan/Log.hpp"
#include "gargantuan/filesystem/BaseFilesystem.hpp"
#include "gargantuan/filesystem/Paths.hpp"

#include <glaze/glaze.hpp>

#include <expected>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace gargantuan {
	const std::unordered_map<std::string, Enums::FontWeight> KEY_TO_WEIGHT = {
		{"100", Enums::FontWeight::Thin},
		{"200", Enums::FontWeight::ExtraLight},
		{"300", Enums::FontWeight::Light},
		{"400", Enums::FontWeight::Regular},
		{"500", Enums::FontWeight::Medium},
		{"600", Enums::FontWeight::SemiBold},
		{"700", Enums::FontWeight::Bold},
		{"800", Enums::FontWeight::ExtraBold},
		{"900", Enums::FontWeight::Heavy},
	};

	void FontProvider::RegisterManifest(const std::filesystem::path &path) {
		if (Filesystem->Type(path) != FileType::File) {
			return LOG_ERROR(App, "Expected font manifest %s to be a file", Paths::ToUtf8(path).c_str());
		}

		auto contents = Filesystem->ReadFileToString(path);

		FontManifest manifest;
		auto error = glz::read_json(manifest, contents);
		if (error) {
			return LOG_ERROR(
				App,
				"Failed to parse font manifest %s: %s",
				Paths::ToUtf8(path).c_str(),
				glz::format_error(error, contents).c_str()
			);
		};

		LOG_VERBOSE(
			App,
			"Loading font %s (family %s) from %s",
			manifest.Name.c_str(),
			manifest.Family.c_str(),
			Paths::ToUtf8(path).c_str()
		);

		for (auto &[weightName, variant] : manifest.Variants) {
			auto foundWeight = KEY_TO_WEIGHT.find(weightName);
			if (foundWeight == KEY_TO_WEIGHT.end()) continue;

			if (auto fontPath = variant.Normal; fontPath.has_value()) {
				RegisterSource(manifest.Name, foundWeight->second, Enums::FontStyle::Normal, path, fontPath.value());
			}

			if (auto fontPath = variant.Italic; fontPath.has_value()) {
				RegisterSource(manifest.Name, foundWeight->second, Enums::FontStyle::Italic, path, fontPath.value());
			}
		}
	}

	void FontProvider::RegisterSource(
		std::string name,
		const Enums::FontWeight &weight,
		const Enums::FontStyle &style,
		const std::filesystem::path &manifestPath,
		const std::filesystem::path &variantPath
	) {
		std::filesystem::path realVariantPath = variantPath.is_relative() ? manifestPath / variantPath : variantPath;
		if (Filesystem->Type(realVariantPath) != FileType::File) {
			return LOG_ERROR(App, "Font variant %s is not a file", Paths::ToUtf8(variantPath).c_str());
		}

		CacheKey sourceKey{name, weight, style};
		Sources[sourceKey] = realVariantPath;
	}

	std::expected<const FontAtlas *, std::string>
	FontProvider::GetAtlas(const std::string &name, const Enums::FontWeight &weight, const Enums::FontStyle &style) {
		CacheKey cacheKey{name, weight, style};

		auto atlas = Atlases.find(cacheKey);
		if (atlas != Atlases.end()) return &atlas->second;

		auto source = Sources.find(cacheKey);
		if (source == Sources.end()) return std::unexpected("font is not registered");

		auto sourceHandle = Filesystem->Open(source->second, FileOpen::Read);
		auto atlasResult = FontAtlas::fromFileHandle(Gpu, sourceHandle);
		sourceHandle->Close();

		if (!atlasResult) return std::unexpected(atlasResult.error().c_str());

		Atlases[cacheKey] = std::move(atlasResult.value());
		return &Atlases[cacheKey];
	}
}
