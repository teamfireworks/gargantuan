#include "gargantuan/assets/FontProvider.hpp"
#include "gargantuan/Log.hpp"
#include "gargantuan/filesystem/BaseFilesystem.hpp"
#include "gargantuan/filesystem/Paths.hpp"

#include <SDL3_ttf/SDL_ttf.h>
#include <glaze/glaze.hpp>

#include <cmath>
#include <exception>
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
		std::vector<std::exception> errors;
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

		SourceKey sourceKey{name, weight, style};
		SourcePaths[sourceKey] = realVariantPath;
	}

	const FontAtlas *FontProvider::Get(
		const std::string &name, const Enums::FontWeight &weight, const Enums::FontStyle &style, uint32_t pointSize
	) {
		AtlasKey atlasKey{name, weight, style, pointSize};
		auto variant = Atlases.find(atlasKey);
		if (variant != Atlases.end()) return &variant->second;

		SourceKey sourceKey{name, weight, style};
		auto source = SourcePaths.find(sourceKey);
		if (source == SourcePaths.end()) {
			LOG_ERROR(App, "Font is not registered");
			return nullptr;
		}

		auto createdVariant = CreateFontAtlas(source->second, pointSize);
		Atlases[atlasKey] = std::move(createdVariant);
		return &Atlases[atlasKey];
	}

	FontAtlas FontProvider::CreateFontAtlas(const std::filesystem::path &source, float pointSize) {
		FontAtlas atlas;

		auto handle = Filesystem->Open(source);
		if (!handle) {
			LOG_ERROR(App, "Failed to open font %s", Paths::ToUtf8(source).c_str());
			return atlas;
		}

		auto rawFont = TTF_OpenFontIO(handle->Stream, false, std::round(pointSize));
		if (!rawFont) {
			LOG_ERROR(App, "Failed to load font %s: %s", Paths::ToUtf8(source).c_str(), SDL_GetError());
			return atlas;
		}

		atlas.LineHeight = TTF_GetFontHeight(rawFont);

		// Todo: Finish ts
		// i gotta do ib math aa homework UGHHHHH
	}
}
