#pragma once

#include "gargantuan/datatypes/Rect.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include <exception>
#include <expected>
#include <filesystem>
#include <unordered_map>

namespace gargantuan {
	struct FontGlyph {
	  public:
		char32_t Codepoint = 0;
		float Advance = 0.0f;

		Rect PlaneBounds{};
		Rect UVBounds{};
	};

	class FontAtlas {
	  public:
		FontAtlas() = default;
		~FontAtlas();

		FontAtlas(const FontAtlas &) = delete;
		FontAtlas &operator=(const FontAtlas &) = delete;

		FontAtlas(FontAtlas &&other) noexcept;
		FontAtlas &operator=(FontAtlas &&other) noexcept;

		static std::expected<FontAtlas, std::exception>
		fromPath(const std::filesystem::path &path, const msdf_atlas::Charset &charset = msdf_atlas::Charset::ASCII);

		SDL_GPUDevice *Gpu;
		SDL_GPUTexture *Texture;
		msdfgen::FontHandle *Font;
		uint32_t Width = 2048, Height = 2048;
		float PixelRange = 4.0;
		float LineHeight = 0.0;

		std::unordered_map<char32_t, FontGlyph> Glyphs;

		const FontGlyph *FetchGlyph(const char32_t codepoint);
	};
}
