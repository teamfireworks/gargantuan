#include "gargantuan/assets/FontAtlas.hpp"

namespace gargantuan {
	FontAtlas::~FontAtlas() {
		if (Gpu && Texture) {
			SDL_ReleaseGPUTexture(Gpu, Texture);
			Texture = nullptr;
		}
	}

	FontAtlas::FontAtlas(FontAtlas &&other) noexcept {
		*this = std::move(other);
	}

	FontAtlas &FontAtlas::operator=(FontAtlas &&other) noexcept {
		if (this != &other) {
			if (Gpu && Texture) {
				SDL_ReleaseGPUTexture(Gpu, Texture);
			}

			Gpu = other.Gpu;
			Texture = other.Texture;
			Width = other.Width;
			Height = other.Height;
			PixelRange = other.PixelRange;
			LineHeight = other.LineHeight;
			Glyphs = std::move(other.Glyphs);

			other.Gpu = nullptr;
			other.Texture = nullptr;
		}
		return *this;
	}
}
