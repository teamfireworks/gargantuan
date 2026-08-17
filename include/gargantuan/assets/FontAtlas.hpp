#pragma once

#include "gargantuan/datatypes/Rect.hpp"
#include "gargantuan/filesystem/BaseFilesystem.hpp"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <msdfgen/ext/import-font.h>

#include <expected>
#include <memory>
#include <unordered_map>
#include <vector>

namespace gargantuan {
	struct FontGlyph {
	  public:
		msdfgen::unicode_t Codepoint = 0;
		float Advance = 0.0f;

		Rect PlaneBounds{};
		Rect AtlasBounds{};
	};

	struct FontAtlas {
	  public:
		FontAtlas() = default;
		~FontAtlas();

		FontAtlas(const FontAtlas &) = delete;
		FontAtlas &operator=(const FontAtlas &) = delete;

		FontAtlas(FontAtlas &&other) noexcept;
		FontAtlas &operator=(FontAtlas &&other) noexcept;

		static std::expected<FontAtlas, std::string> fromFontBuffer(
			SDL_GPUDevice *gpu,
			std::vector<uint8_t> &fontBuffer,
			const uint32_t fontBytes,
			const msdf_atlas::Charset &charset = msdf_atlas::Charset::ASCII
		);

		static std::expected<FontAtlas, std::string> fromFileHandle(
			SDL_GPUDevice *gpu,
			const std::unique_ptr<FileHandle> &handle,
			const msdf_atlas::Charset &charset = msdf_atlas::Charset::ASCII
		);

		SDL_GPUDevice *Gpu;
		SDL_GPUTexture *Texture;
		msdfgen::FontHandle *Font;
		uint32_t Width = 2048, Height = 2048;
		double PixelRange = 4.0;

		msdfgen::FontMetrics Metrics;

		std::unordered_map<msdfgen::unicode_t, FontGlyph> Glyphs;
		const FontGlyph *GetGlyph(const msdfgen::unicode_t codepoint);
	};
}
