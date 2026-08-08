#pragma once

#include "gargantuan/classes/GuiBase2d.hpp"
#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/datatypes/Rect.hpp"
#include "gargantuan/datatypes/UDim.hpp"
#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/reflection/Enums.hpp"
#include <memory>
#include <optional>

namespace gargantuan {
	G_ENUM(GuiState, Idle, Hover, Press, NonInteractable);
	G_ENUM(InputSink, None, Activate, All = 100);
	G_ENUM(SizeConstaint, RelativeXX, RelativeXY, RelativeYY);

	class GuiObject : public GuiBase2d {
	  public:
		G_INSTANCE_DECL(GuiObject);

		Vector2 AnchorPoint{0.0f, 0.0f};
		Color3 BackgroundColor3{1.0f, 1.0f, 1.0f};
		float BackgroundTransparency{0.0f};
		bool ClipsDescendants{true};
		Enums::GuiState GuiState{Enums::GuiState::NonInteractable};
		Enums::InputSink InputSink{Enums::InputSink::None};
		bool Interactable{false};
		int LayoutOrder{0};
		UDim Position{0.0f, 0};
		float Rotation{0.0f};
		bool Selectable{false};
		std::optional<std::shared_ptr<GuiObject>> SelectionImageObject = std::nullopt;
		int SelectionOrder{0};
		UDim Size{0.0f, 0};
		Enums::SizeConstaint SizeConstaint{Enums::SizeConstaint::RelativeXY};
		bool Visible{true};
		int ZIndex{0};

		Rect AbsoluteBounds{};
		bool AbsoluteBoundsDirty{true};
		Rect CalculateAbsoluteBounds();
	};
}
