#include "gargantuan/assets/FontAtlas.hpp"
#include "gargantuan/datatypes/Vector2.hpp"

#include <core/BitmapRef.hpp>
#include <cstddef>
#include <expected>
#include <ext/import-font.h>
#include <msdf-atlas-gen/FontGeometry.h>
#include <msdf-atlas-gen/GlyphGeometry.h>
#include <msdf-atlas-gen/glyph-generators.h>
#include <vector>

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
			Metrics = other.Metrics;
			Glyphs = std::move(other.Glyphs);

			other.Gpu = nullptr;
			other.Texture = nullptr;
		}
		return *this;
	}

	std::expected<FontAtlas, std::string> FontAtlas::fromFontBuffer(
		SDL_GPUDevice *gpu,
		std::vector<uint8_t> &fontBuffer,
		const uint32_t fontBytes,
		const msdf_atlas::Charset &charset
	) {
		if (!gpu) return std::unexpected("no GPU");

		auto ft = msdfgen::initializeFreetype();
		if (!ft) return std::unexpected("msdfgen failed to initialize the FreeType library");

		auto font = msdfgen::loadFontData(ft, reinterpret_cast<const msdfgen::byte *>(fontBuffer.data()), fontBytes);
		if (!font) {
			msdfgen::deinitializeFreetype(ft);
			return std::unexpected("msdfgen failed to parse font buffer");
		};

		// Packing

		std::vector<msdf_atlas::GlyphGeometry> glyphs;
		msdf_atlas::FontGeometry fontGeometry(&glyphs);
		fontGeometry.loadCharset(font, 1.0, charset);

		const double maxCornerAngle = 3.0;
		for (auto &glyph : glyphs) {
			glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
		}

		const double glyphScale = 32.0;
		const double pixelRange = 4.0;

		msdf_atlas::TightAtlasPacker packer;
		packer.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::SQUARE);
		packer.setMinimumScale(glyphScale);
		packer.setPixelRange(pixelRange);
		packer.setMiterLimit(1.0);
		packer.pack(glyphs.data(), glyphs.size());

		int width = 0, height = 0;
		packer.getDimensions(width, height);

		msdf_atlas::ImmediateAtlasGenerator<
			float,
			4,
			msdf_atlas::mtsdfGenerator,
			msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 4>>
			generator(width, height);

		msdf_atlas::GeneratorAttributes attributes;
		generator.setAttributes(attributes);
		generator.setThreadCount(4);

		generator.generate(glyphs.data(), glyphs.size());

		// Atlas

		SDL_GPUTextureCreateInfo textureInfo{
			.type = SDL_GPU_TEXTURETYPE_2D,
			.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
			.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
			.width = static_cast<Uint32>(width),
			.height = static_cast<Uint32>(height),
			.layer_count_or_depth = 1,
			.num_levels = 1,
		};

		SDL_GPUTexture *gpuTexture = SDL_CreateGPUTexture(gpu, &textureInfo);
		if (!gpuTexture) {
			msdfgen::destroyFont(font);
			msdfgen::deinitializeFreetype(ft);
			return std::unexpected(std::string("failed to create GPU texture: ") + SDL_GetError());
		}

		// msdfgen::BitmapConstRef<msdf_atlas::byte, 3> bitmap = generator.atlasStorage();
		// std::vector<uint8_t> rgbaBuffer(width * height * 4);
		// for (int i = 0; i < width * height; ++i) {
		// 	rgbaBuffer[i * 4 + 0] = bitmap.pixels[i * 3 + 0];
		// 	rgbaBuffer[i * 4 + 1] = bitmap.pixels[i * 3 + 1];
		// 	rgbaBuffer[i * 4 + 2] = bitmap.pixels[i * 3 + 2];
		// 	rgbaBuffer[i * 4 + 3] = 255;
		// }

		msdfgen::BitmapConstRef<msdf_atlas::byte, 4> bitmap = generator.atlasStorage();
		Uint32 uploadSize = static_cast<Uint32>(width * height * 4);

		SDL_GPUTransferBufferCreateInfo transferInfo{.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = uploadSize};
		SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(gpu, &transferInfo);

		void *mapped = SDL_MapGPUTransferBuffer(gpu, transferBuffer, false);
		std::memcpy(mapped, bitmap.pixels, uploadSize);
		SDL_UnmapGPUTransferBuffer(gpu, transferBuffer);

		SDL_GPUCommandBuffer *commands = SDL_AcquireGPUCommandBuffer(gpu);
		SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commands);

		SDL_GPUTextureTransferInfo sourceInfo{.transfer_buffer = transferBuffer, .offset = 0};
		SDL_GPUTextureRegion destinationRegion{
			.texture = gpuTexture,
			.w = static_cast<Uint32>(width),
			.h = static_cast<Uint32>(height),
			.d = 1,
		};
		SDL_UploadToGPUTexture(copyPass, &sourceInfo, &destinationRegion, false);

		SDL_EndGPUCopyPass(copyPass);
		SDL_SubmitGPUCommandBuffer(commands);
		SDL_ReleaseGPUTransferBuffer(gpu, transferBuffer);

		// Assembly

		FontAtlas self;
		self.Gpu = gpu;
		self.Texture = gpuTexture;
		self.Font = font;
		self.Width = width;
		self.Height = height;
		self.PixelRange = pixelRange;

		msdfgen::getFontMetrics(self.Metrics, font);

		for (auto &glyphGeometry : glyphs) {
			FontGlyph glyph;
			glyph.Codepoint = glyphGeometry.getCodepoint();
			glyph.Advance = glyphGeometry.getAdvance();

			double bl, bb, br, bt;

			glyphGeometry.getQuadPlaneBounds(bl, bb, br, bt);
			glyph.PlaneBounds.Min = Vector2(bl, bb);
			glyph.PlaneBounds.Max = Vector2(br, bt);

			glyphGeometry.getQuadAtlasBounds(bl, bb, br, bt);
			glyph.AtlasBounds.Min = Vector2(bl, bb);
			glyph.AtlasBounds.Max = Vector2(br, bt);

			self.Glyphs[glyph.Codepoint] = std::move(glyph);
		}

		msdfgen::destroyFont(font);
		msdfgen::deinitializeFreetype(ft);

		return self;
	};

	std::expected<FontAtlas, std::string> FontAtlas::fromFileHandle(
		SDL_GPUDevice *gpu, const std::unique_ptr<FileHandle> &handle, const msdf_atlas::Charset &charset
	) {
		std::vector<uint8_t> fontBuffer;
		auto fontBytes = handle->Size();
		fontBuffer.reserve(fontBytes);
		handle->Read(&fontBuffer, fontBytes);

		return fromFontBuffer(gpu, fontBuffer, fontBytes, charset);
	};

	const FontGlyph *FontAtlas::GetGlyph(const msdfgen::unicode_t codepoint) {
		return Glyphs.contains(codepoint) ? &Glyphs[codepoint] : nullptr;
	};
}
