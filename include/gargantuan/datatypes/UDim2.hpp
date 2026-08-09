#include "gargantuan/datatypes/UDim.hpp"

#include <glm/glm.hpp>

namespace gargantuan {
	G_USERDATA_DECL(
		UDim2,

		UDim X{0.0f, 0};
		UDim Y{0.0f, 0};

		UDim2(UDim x, UDim y);
		UDim2(float xScale = 0.0f, int xOffset = 0, float yScale = 0.0f, int yOffset = 0);
		static UDim2 fromScale(float x, float y);
		static UDim2 fromOffset(int x, int y);

		UDim2 Lerp(const UDim2 &goal, float alpha) const;
		UDim2 Add(const UDim2 &other) const;
		UDim2 Sub(const UDim2 &other) const;
		UDim2 Unm() const;
		bool Eq(const UDim2 &other) const;
		std::string Tostring() const;
	)
}
