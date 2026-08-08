#pragma once

#include "gargantuan/classes/GuiBase2d.hpp"
#include "gargantuan/classes/GuiObject.hpp"
#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/reflection/Enums.hpp"

#include <memory>
#include <set>

namespace gargantuan {
	G_ENUM(ZIndexBehavior, Sibling, Global);

	class LayerCollector : public GuiBase2d {
	  public:
		G_INSTANCE_DECL(LayerCollector);

		LayerCollector();

		bool Enabled{true};
		bool ResetOnSpawn{false};
		Enums::ZIndexBehavior ZIndexBehavior{Enums::ZIndexBehavior::Sibling};

		std::set<std::shared_ptr<GuiObject>> GuiObjects;
		void Collect();

	  private:
		void CollectDescendant(std::shared_ptr<Instance> descendant);
	};
}
