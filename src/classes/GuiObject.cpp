#include "gargantuan/classes/GuiObject.hpp"

namespace gargantuan {
	Rect GuiObject::CalculateAbsoluteBounds() {
		if (!AbsoluteBoundsDirty) return AbsoluteBounds;

		auto parent = FindFirstAncestorWhichIsA("GuiBase2d");
		if (parent) {
			// ???????
		}

		AbsoluteBoundsDirty = false;
		return AbsoluteBounds;
	}
}
