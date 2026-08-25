#include "gargantuan/classes/Constraint.hpp"
#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/math/Conversions.hpp"
#include <box3d/box3d.h>
#include <initializer_list>
#include <memory>
#include <string>
#include <variant>

namespace gargantuan {
	std::tuple<std::shared_ptr<BasePart>, std::shared_ptr<BasePart>> Constraint::GetActiveParts() const {
		if (!Attachment0.has_value() || !Attachment1.has_value()) return {nullptr, nullptr};

		auto maybePart0 = Attachment0.value()->ParentPointer;
		if (!maybePart0 || !maybePart0->FindFirstAncestorWhichIsA("WorldRoot")) return {nullptr, nullptr};
		auto part0 = std::dynamic_pointer_cast<BasePart>(maybePart0->shared_from_this());
		if (!part0) return {nullptr, nullptr};

		auto maybePart1 = Attachment1.value()->ParentPointer;
		if (!maybePart1 || !maybePart1->FindFirstAncestorWhichIsA("WorldRoot")) return {nullptr, nullptr};
		auto part1 = std::dynamic_pointer_cast<BasePart>(maybePart1->shared_from_this());
		if (!part1) return {nullptr, nullptr};

		return {part0, part1};
	};

	Constraint::Constraint() {
		BindStructuralProperties({"Attachment0", "Attachment1", "Enabled"});
	}

	void Constraint::BindJointProperties(std::initializer_list<std::string> propertyNames) {
		auto onChanged = [this](std::monostate) {
			if (!b3Joint_IsValid(LeJoint)) return;
			UpdateJoint();
		};

		for (const auto &propertyName : propertyNames) {
			GetPropertyChangedSignal(propertyName)->Connect(onChanged);
		}
	}

	void Constraint::BindStructuralProperties(std::initializer_list<std::string> propertyNames) {
		auto onChanged = [this](std::monostate) {
			if (RequestRebuild) RequestRebuild();
		};

		for (const auto &propertyName : propertyNames) {
			GetPropertyChangedSignal(propertyName)->Connect(onChanged);
		}
	}

	void Constraint::UpdateJointFrames() {
		if (Attachment0.has_value()) {
			auto cframe = Attachment0.value()->GetCFrame();
			b3Joint_SetLocalFrameA(LeJoint, {AsB3Vec3(cframe.Position), AsB3Quat(cframe.ToQuaternion())});
		}

		if (Attachment1.has_value()) {
			auto cframe = Attachment1.value()->GetCFrame();
			b3Joint_SetLocalFrameB(LeJoint, {AsB3Vec3(cframe.Position), AsB3Quat(cframe.ToQuaternion())});
		}
	}

	bool Constraint::GetActive() const {
		if (!GetEnabled()) return false;

		auto [part0, part1] = GetActiveParts();
		return part0 && part1;
	}
}
