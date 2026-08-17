#include "gargantuan/assets/FontAtlas.hpp"
#include "gargantuan/classes/GuiObject.hpp"
#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/classes/TextLabel.hpp"
#include "gargantuan/render/RenderPrimitives.hpp"
#include "gargantuan/render/Renderer.hpp"

#include <SDL3/SDL.h>
#include <cstdint>
#include <memory>
#include <vector>

namespace gargantuan {
	class GuiPass final : public RenderPass {
	  public:
		static constexpr const char *LABEL = "Gui";

		struct alignas(16) Uniforms {
			glm::vec2 ViewportSize;
		};

		struct TextureBatch {
			GuiTextureType Type = GuiTextureType::Color;
			SDL_GPUTexture *Texture = nullptr;
			uint32_t IndexOffset;
			uint32_t IndexCount = 0;
		};

		FileShader Shader{
			.VertexFilepath = GetShaderPath("gui.vert"),
			.VertexUniformBufferCount = 1,
			.FragmentFilepath = GetShaderPath("gui.frag"),
			.FragmentUniformBufferCount = 0,
		};

		std::vector<GuiVertex> Vertices;
		std::vector<uint32_t> Indices;
		std::vector<TextureBatch> Batches;

		SDL_GPUDevice *Gpu;
		SDL_GPUSampler *Sampler = nullptr;

		GuiPass(SDL_GPUDevice *gpu, SDL_GPUTextureFormat swapchainFormat) : Gpu(gpu) {
			Shader.Init(gpu);

			SDL_GPUGraphicsPipelineCreateInfo info{};
			info.vertex_shader = Shader.VertexShader;
			info.fragment_shader = Shader.FragmentShader;

			info.vertex_input_state.vertex_attributes = GuiVertex::Attributes->data();
			info.vertex_input_state.num_vertex_attributes = static_cast<Uint32>(GuiVertex::Attributes->size());
			info.vertex_input_state.vertex_buffer_descriptions = GuiVertex::BufferDescriptions->data();
			info.vertex_input_state.num_vertex_buffers = static_cast<Uint32>(GuiVertex::BufferDescriptions->size());

			info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
			info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
			info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
			info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;

			info.depth_stencil_state.enable_depth_test = false;
			info.depth_stencil_state.enable_depth_write = false;

			SDL_GPUColorTargetDescription colorTarget{};
			{
				colorTarget.format = swapchainFormat;
				colorTarget.blend_state.enable_blend = true;
				colorTarget.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
				colorTarget.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
				colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
				colorTarget.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
				colorTarget.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
				colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
			}
			info.target_info.num_color_targets = 1;
			info.target_info.color_target_descriptions = &colorTarget;

			info.target_info.depth_stencil_format = RENDERER_DEPTH_FORMAT;
			info.target_info.has_depth_stencil_target = false;

			Pipeline = SDL_CreateGPUGraphicsPipeline(gpu, &info);

			SDL_GPUSamplerCreateInfo samplerInfo{
				.min_filter = SDL_GPU_FILTER_LINEAR,
				.mag_filter = SDL_GPU_FILTER_LINEAR,
				.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,

				.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
				.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
				.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			};

			Sampler = SDL_CreateGPUSampler(gpu, &samplerInfo);
		};

		~GuiPass() {
			if (Gpu && Sampler) {
				SDL_ReleaseGPUSampler(Gpu, Sampler);
				Sampler = nullptr;
			}

			Gpu = nullptr;
		}

		SDL_GPURenderPass *Draw(SDL_GPUDevice *gpu, FrameContext &context) override {
			for (auto &layer : context.Layers) {
				CollectGuiObjects(context, layer);
			}

			if (Batches.empty()) return nullptr;

			SDL_GPUBuffer *vertexBuffer, *indexBuffer;
			UploadBuffers(gpu, context, vertexBuffer, indexBuffer);

			SDL_GPUColorTargetInfo colorTarget = {
				.texture = context.SwapchainTexture,
				.load_op = SDL_GPU_LOADOP_LOAD,
				.store_op = SDL_GPU_STOREOP_STORE,
			};

			SDL_GPURenderPass *pass = SDL_BeginGPURenderPass(context.Commands, &colorTarget, 1, nullptr);
			SDL_BindGPUGraphicsPipeline(pass, Pipeline);

			Uniforms uniforms{
				.ViewportSize = {
					static_cast<float>(context.Width),
					static_cast<float>(context.Height),
				}
			};
			SDL_PushGPUVertexUniformData(context.Commands, 0, &uniforms, sizeof(Uniforms));

			SDL_GPUBufferBinding vertexBinding{.buffer = vertexBuffer, .offset = 0};
			SDL_BindGPUVertexBuffers(pass, 0, &vertexBinding, 1);

			SDL_GPUBufferBinding indexBinding{.buffer = indexBuffer, .offset = 0};
			SDL_BindGPUIndexBuffer(pass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

			for (auto &batch : Batches) {
				if (batch.Texture) {
					SDL_GPUTextureSamplerBinding samplerBinding{
						.texture = batch.Texture,
						.sampler = batch.Type == GuiTextureType::Font ? Sampler : nullptr,
					};
					SDL_BindGPUFragmentSamplers(pass, 0, &samplerBinding, 1);
				}
				SDL_DrawGPUIndexedPrimitives(pass, batch.IndexCount, 1, batch.IndexOffset, 0, 0);
			}

			SDL_ReleaseGPUBuffer(gpu, vertexBuffer);
			SDL_ReleaseGPUBuffer(gpu, indexBuffer);

			Vertices.clear();
			Indices.clear();
			Batches.clear();

			return pass;
		};

	  private:
		void CollectGuiObjects(FrameContext &context, std::shared_ptr<Instance> parent) {
			for (auto &child : parent->GetChildren()) {
				auto guiObject = std::dynamic_pointer_cast<GuiObject>(child);
				if (!guiObject || !guiObject->GetVisible()) continue;

				if (auto textLabel = std::dynamic_pointer_cast<TextLabel>(child)) {
					auto font = textLabel->GetFontFace();

					auto expectedAtlas = context.Font->GetAtlas(font.Family, font.Weight, font.Style);
					if (!expectedAtlas.has_value()) continue;

					auto fontAtlas = expectedAtlas.value();
					auto batch = GetTextureBatch(fontAtlas->Texture, GuiTextureType::Font);
					batch->IndexCount += PushBackground(guiObject, GuiTextureType::Color);
					batch->IndexCount += PushText(textLabel, fontAtlas);
				} else {
					auto batch = GetTextureBatch(nullptr, GuiTextureType::Color);
					batch->IndexCount += PushBackground(guiObject, GuiTextureType::Color);
				}

				CollectGuiObjects(context, child);
			}
		}

		TextureBatch *GetTextureBatch(SDL_GPUTexture *texture, GuiTextureType type) {
			if (!Batches.empty()) {
				auto &current = Batches.back();
				if (current.Type == type && current.Texture == texture) {
					return &current;
				}
			}

			Batches.push_back({
				.Type = type,
				.Texture = texture,
				.IndexOffset = static_cast<uint32_t>(Indices.size()),
				.IndexCount = 0,
			});

			return &Batches.back();
		}

		uint32_t PushBackground(GuiObject::Pointer object, GuiTextureType textureType) {
			Rect bounds = object->CalculateAbsoluteBounds();
			uint32_t baseIndex = static_cast<uint32_t>(Vertices.size());

			glm::vec2 min = bounds.Min, max = bounds.Max;
			glm::vec2 size = bounds.GetSize();
			glm::vec4 color = {
				(glm::vec3)object->GetBackgroundColor3(),
				1.0f - object->GetBackgroundTransparency(),
			};
			float rotation = object->GetRotation();

			Vertices.push_back({min, size, {0.0f, 0.0f}, color, rotation, textureType});
			Vertices.push_back({{max.x, min.y}, size, {1.0f, 0.0f}, color, rotation, textureType});
			Vertices.push_back({{min.x, max.y}, size, {0.0f, 1.0f}, color, rotation, textureType});
			Vertices.push_back({max, size, {1.0f, 1.0f}, color, rotation, textureType});

			Indices.push_back(baseIndex + 0);
			Indices.push_back(baseIndex + 2);
			Indices.push_back(baseIndex + 1);

			Indices.push_back(baseIndex + 1);
			Indices.push_back(baseIndex + 2);
			Indices.push_back(baseIndex + 3);

			return 6;
		}

		uint32_t PushText(TextLabel::Pointer object, const FontAtlas *fontAtlas) {
			Rect bounds = object->CalculateAbsoluteBounds();
			float scale = object->GetTextSize() / object->GetLineHeight();
			uint32_t indexCount = 0;

			glm::vec2 cursor = bounds.Min + glm::vec2(0.0f, fontAtlas->Metrics.ascenderY * scale);
			glm::vec4 textColor = {(glm::vec3)object->GetTextColor3(), 1.0f - object->GetTextTransparency()};

			for (auto c : object->GetText()) {
				auto glyph = fontAtlas->GetGlyph(c);
				if (!glyph) continue;

				auto minX = glyph->PlaneBounds.Min.GetX(), minY = glyph->PlaneBounds.Min.GetY();
				auto maxX = glyph->PlaneBounds.Max.GetX(), maxY = glyph->PlaneBounds.Max.GetY();

				glm::vec2 minPos = cursor + glm::vec2(minX, -maxY) * scale;
				glm::vec2 maxPos = cursor + glm::vec2(maxX, -minY) * scale;
				glm::vec2 quadSize = maxPos - minPos;

				uint32_t baseIndex = static_cast<uint32_t>(Vertices.size());

				minX = glyph->AtlasBounds.Min.GetX(), minY = glyph->AtlasBounds.Min.GetY();
				maxX = glyph->AtlasBounds.Max.GetX(), maxY = glyph->AtlasBounds.Max.GetY();

				Vertices.push_back({
					minPos,
					quadSize,
					glyph->AtlasBounds.Min,
					textColor,
					0.0f,
					GuiTextureType::Font,
				});

				Vertices.push_back({
					{maxPos.x, minPos.y},
					quadSize,
					{maxX, maxY},
					textColor,
					0.0f,
					GuiTextureType::Font,
				});

				Vertices.push_back({
					{minPos.x, maxPos.y},
					quadSize,
					{minX, maxY},
					textColor,
					0.0f,
					GuiTextureType::Font,
				});

				Vertices.push_back({
					maxPos,
					quadSize,
					glyph->AtlasBounds.Max,
					textColor,
					0.0f,
					GuiTextureType::Font,
				});

				Indices.push_back(baseIndex + 0);
				Indices.push_back(baseIndex + 2);
				Indices.push_back(baseIndex + 1);

				Indices.push_back(baseIndex + 1);
				Indices.push_back(baseIndex + 2);
				Indices.push_back(baseIndex + 3);

				indexCount += 6;
				cursor.x += glyph->Advance * scale;
			}

			return indexCount;
		}

		void UploadBuffers(
			SDL_GPUDevice *gpu, FrameContext &context, SDL_GPUBuffer *&vertexBuffer, SDL_GPUBuffer *&indexBuffer
		) {
			uint32_t vertexBufferSize = static_cast<uint32_t>(Vertices.size() * sizeof(GuiVertex));
			uint32_t indexBufferSize = static_cast<uint32_t>(Indices.size() * sizeof(uint32_t));

			SDL_GPUBufferCreateInfo vertexBufferInfo{.usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = vertexBufferSize};
			SDL_GPUBufferCreateInfo indexBufferInfo{.usage = SDL_GPU_BUFFERUSAGE_INDEX, .size = indexBufferSize};
			vertexBuffer = SDL_CreateGPUBuffer(gpu, &vertexBufferInfo);
			indexBuffer = SDL_CreateGPUBuffer(gpu, &indexBufferInfo);

			SDL_GPUTransferBufferCreateInfo tBufferInfo{
				.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
				.size = vertexBufferSize + indexBufferSize,
			};
			SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(gpu, &tBufferInfo);

			uint8_t *mapped = static_cast<uint8_t *>(SDL_MapGPUTransferBuffer(gpu, transferBuffer, false));
			std::memcpy(mapped, Vertices.data(), vertexBufferSize);
			std::memcpy(mapped + vertexBufferSize, Indices.data(), indexBufferSize);
			SDL_UnmapGPUTransferBuffer(gpu, transferBuffer);

			SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(context.Commands);

			SDL_GPUTransferBufferLocation vertexSource{.transfer_buffer = transferBuffer, .offset = 0};
			SDL_GPUBufferRegion vertexDestination{.buffer = vertexBuffer, .offset = 0, .size = vertexBufferSize};
			SDL_UploadToGPUBuffer(copyPass, &vertexSource, &vertexDestination, false);

			SDL_GPUTransferBufferLocation indexSource{.transfer_buffer = transferBuffer, .offset = vertexBufferSize};
			SDL_GPUBufferRegion indexDestination{.buffer = indexBuffer, .offset = 0, .size = indexBufferSize};
			SDL_UploadToGPUBuffer(copyPass, &indexSource, &indexDestination, false);

			SDL_EndGPUCopyPass(copyPass);
			SDL_ReleaseGPUTransferBuffer(gpu, transferBuffer);
		}
	};

	std::unique_ptr<RenderPass> CreateGuiPass(SDL_GPUDevice *gpu, SDL_GPUTextureFormat swapchainFormat) {
		return std::make_unique<GuiPass>(gpu, swapchainFormat);
	}
}
