#pragma once

#include "gargantuan/classes/generated/Part.hpp"

namespace gargantuan {
	G_ENUM(
		PartType,

		Ball,
		Block,
		Cylinder,
		Wedge,
		CornerWedge
	)

	class Part : public BasePart {
		I_Part;

		std::unique_ptr<GpuMesh> &GetMesh() const override;
	};
} // namespace gargantuan
