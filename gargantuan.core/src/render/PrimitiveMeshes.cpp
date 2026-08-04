// Has GPU programming gone too far?

#include "gargantuan.core/render/PrimitiveMeshes.hpp"

#include <glm/glm.hpp>

namespace gargantuan::PrimitiveMeshes {
	static constexpr glm::vec2 UV_00{0.0f, 1.0f};
	static constexpr glm::vec2 UV_01{0.0f, 1.0f};
	static constexpr glm::vec2 UV_10{1.0f, 0.0f};
	static constexpr glm::vec2 UV_11{1.0f, 1.0f};

	Mesh Block() {
		return Mesh{
			std::vector<Vertex>{
				Vertex{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, UV_00},
				Vertex{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, UV_01},
				Vertex{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, UV_11},
				Vertex{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, UV_10},

				Vertex{{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, UV_00},
				Vertex{{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, UV_01},
				Vertex{{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, UV_11},
				Vertex{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, UV_10},

				Vertex{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, UV_00},
				Vertex{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, UV_01},
				Vertex{{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, UV_11},
				Vertex{{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, UV_10},

				Vertex{{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, UV_00},
				Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, UV_01},
				Vertex{{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, UV_11},
				Vertex{{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, UV_10},

				Vertex{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, UV_00},
				Vertex{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, UV_01},
				Vertex{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, UV_11},
				Vertex{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, UV_10},

				Vertex{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, UV_00},
				Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, UV_01},
				Vertex{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, UV_11},
				Vertex{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, UV_10},
			},
			std::vector<uint32_t>{
				0,	1,	2,	0,	2,	3,	4,	5,	6,	4,	6,	7,	8,	9,	10, 8,	10, 11,
				12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23,
			},
		};
	};
	Mesh Wedge() {
		return Mesh{
			{
				Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, UV_00},
				Vertex{{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, UV_10},
				Vertex{{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, UV_11},
				Vertex{{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, UV_01},

				Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, UV_00},
				Vertex{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, UV_10},
				Vertex{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, UV_11},
				Vertex{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, UV_01},

				Vertex{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, UV_00},
				Vertex{{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, UV_10},
				Vertex{{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, UV_01},

				Vertex{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, UV_00},
				Vertex{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, UV_10},
				Vertex{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, UV_01},

				Vertex{{-0.5f, 0.5f, -0.5f}, {0.707f, 0.707f, 0.707f}, UV_00},
				Vertex{{0.5f, 0.5f, -0.5f}, {0.707f, 0.707f, 0.707f}, UV_10},
				Vertex{{0.5f, -0.5f, 0.5f}, {0.707f, 0.707f, 0.707f}, UV_11},
				Vertex{{-0.5f, -0.5f, 0.5f}, {0.707f, 0.707f, 0.707f}, UV_01},
			},
			{
				0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6, 8, 9, 10, 11, 13, 12, 14, 16, 15, 14, 17, 16,
			},
		};
	}
} // namespace gargantuan::PrimitiveMeshes
