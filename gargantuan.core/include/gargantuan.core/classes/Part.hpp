#pragma once

#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/reflection/Enums.hpp"

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
	  public:
		G_INSTANCE_DECL(Part);

		Enums::PartType Shape = Enums::PartType::Block;
		std::unique_ptr<GpuMesh> &GetMesh() const override;
	};
} // namespace gargantuan
