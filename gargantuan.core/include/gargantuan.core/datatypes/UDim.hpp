#include "gargantuan.runtime/Userdata.hpp"
#include <glm/glm.hpp>
#include <lua.h>

namespace gargantuan {
	G_USERDATA_DECL(
		UDim,

		float Scale = 0.0f;
		int Offset = 0;

		UDim(float scale = 0.0f, int offset = 0);

		UDim Lerp(const UDim &goal, float alpha) const;
		UDim Add(const UDim &other) const;
		UDim Sub(const UDim &other) const;
		UDim Unm() const;
		bool Eq(const UDim &other) const;
		std::string Tostring() const;
	)
}
