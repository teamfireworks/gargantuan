#pragma once

#include "gargantuan/assets/FontAtlas.hpp"
#include "gargantuan/filesystem/BaseFilesystem.hpp"
#include "gargantuan/reflection/Enums.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/glm.hpp>

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>

namespace gargantuan {
	G_ENUM(
		FontWeight,
		Thin = 100,
		ExtraLight = 200,
		Light = 300,
		Regular = 400,
		Medium = 500,
		SemiBold = 600,
		Bold = 700,
		ExtraBold = 800,
		Heavy = 900
	);

	G_ENUM(FontStyle, Normal, Italic);

	struct FontManifestVariant {
		std::optional<std::string> Normal;
		std::optional<std::string> Italic;
	};

	struct FontManifest {
		std::string Name;
		std::string Family;
		std::unordered_map<std::string, FontManifestVariant> Variants;
	};

	class FontProvider {
	  public:
		FontProvider(SDL_GPUDevice *gpu, BaseFilesystem *filesystem) : Gpu(gpu), Filesystem(filesystem) {};
		~FontProvider();

		void RegisterManifest(const std::filesystem::path &path);

		std::expected<const FontAtlas *, std::string>
		GetAtlas(const std::string &name, const Enums::FontWeight &weight, const Enums::FontStyle &style);

	  private:
		SDL_GPUDevice *Gpu;
		BaseFilesystem *Filesystem;

		using CacheKey = std::tuple<std::string, Enums::FontWeight, Enums::FontStyle>;
		std::map<CacheKey, std::filesystem::path> Sources;
		std::map<CacheKey, FontAtlas> Atlases;

		void RegisterSource(
			std::string name,
			const Enums::FontWeight &weight,
			const Enums::FontStyle &style,
			const std::filesystem::path &manifestPath,
			const std::filesystem::path &variantPath
		);
	};
}
