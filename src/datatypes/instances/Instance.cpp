#include "gargantuan/datatypes/instances/Instance.hpp"
#include "gargantuan/datatypes/instances/InstanceProperty.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"

#include <memory>
#include <optional>

namespace gargantuan {
	I_IMPL(
		Instance,
		.Description = "Abstract class for most instance classes.",
		.Register =
			[]() {
				Instance::Parent.RawRead = [](Instance *self) -> std::optional<std::shared_ptr<Instance>> {
					return self->ParentPointer
							   ? std::optional<std::shared_ptr<Instance>>(self->ParentPointer->shared_from_this())
							   : std::nullopt;
				};

				Instance::Parent.RawWrite = [](Instance *self, std::optional<std::shared_ptr<Instance>> parent) {
					self->SetParent(parent.has_value() ? parent.value() : nullptr);
				};
			},
		.Methods = {
			{"IsA", Method::fromMember<&Instance::IsA>()},
			{"GetFullName", Method::fromMember<&Instance::GetFullName>()},
			{"GetChildren", Method::fromMember<&Instance::GetChildren>()},
			{"GetDescendants", Method::fromMember<&Instance::GetDescendants>()},
			{"FindFirstChild", Method::fromMember<&Instance::FindFirstChild>()},
			{"FindFirstChildOfClass", Method::fromMember<&Instance::FindFirstChildOfClass>()},
		}
	)

	void Instance::AssertNotDestroyed() {
		if (Destroyed[this]) throw "Instance is destroyed";
	}

	void Instance::SetParent(std::shared_ptr<Instance> newParent) {
		AssertNotDestroyed();
		if (ParentPointer == newParent.get()) return;

		std::shared_ptr<Instance> self = shared_from_this();

		// This whole subtree leaves the old ancestry and joins the new one, so
		// collect it once up front and reuse it for both sets of signals
		std::vector<std::shared_ptr<Instance>> subtree = {self};
		CollectDescendants(subtree);

		if (ParentPointer != nullptr) {
			auto &oldChildren = ParentPointer->Children;
			if (auto it = std::find(oldChildren.begin(), oldChildren.end(), self); it != oldChildren.end()) {
				oldChildren.erase(it);
				ParentPointer->ChildRemoved->Fire(self);
			}

			for (Instance *ancestor = ParentPointer; ancestor != nullptr; ancestor = ancestor->ParentPointer) {
				for (auto &node : subtree) {
					ancestor->DescendantRemoved->Fire(node);
				}
			}
		}

		Parent.Write(this, newParent);

		if (newParent != nullptr) {
			newParent->Children.push_back(self);
			newParent->ChildAdded->Fire(self);

			for (Instance *ancestor = newParent.get(); ancestor != nullptr; ancestor = ancestor->ParentPointer) {
				for (auto &node : subtree) {
					ancestor->DescendantAdded->Fire(node);
				}
			}
		}

		FireAncestryChanged(self, newParent);
	}

	void Instance::FireAncestryChanged(std::shared_ptr<Instance> child, std::shared_ptr<Instance> parent) {
		AncestryChanged->Fire({child, parent});
		for (auto &descendant : Children) {
			descendant->FireAncestryChanged(child, parent);
		}
	}

	void Instance::ClearAllChildren() {
		AssertNotDestroyed();
		auto children = Children;
		for (auto &child : children) {
			child->Destroy();
		}
	}

	void Instance::Destroy() {
		if (Destroyed[this]) throw "Attempt to destroy twice";

		Destroying->Fire({});

		auto children = Children;
		for (auto &child : children) {
			child->Destroy();
		}

		SetParent(nullptr);
		Destroyed.Write(this, true);
		Destroying->Fire({});
	}

	InstanceProperty<std::any> *Instance::FindProperty(std::string_view name) {
		const InstanceClassDefinition *definition = InstanceClassRegistry::GetDefinition(this);
		if (!definition) {
			return nullptr;
		}

		auto it = definition->AllProperties.find(name);
		return it != definition->AllProperties.end() ? it->second : nullptr;
	}

	const Instance::Self::Method *Instance::FindMethod(std::string_view name) {
		const InstanceClassDefinition *definition = InstanceClassRegistry::GetDefinition(this);
		if (!definition) {
			return nullptr;
		}

		auto it = definition->AllMethods.find(name);
		return it != definition->AllMethods.end() ? it->second : nullptr;
	}

	int Instance::LIndex(lua_State *L, Instance *self) {
		const char *key = luaL_checkstring(L, 2);

		if (key && self) {
			auto *property = self->FindProperty(key);
			if (property) {
				if (property->IsReadImplemented()) {
					return property->PushStack(L, property->Read(self));
				} else {
					luaL_error(L, "Property %s is write-only", key);
				}
			} else if (auto child = self->FindFirstChild(key)) {
				StackValue<std::shared_ptr<Instance>>::Push(L, child);
				return 1;
			}
		}

		return 0;
	};

	int Instance::LNewIndex(lua_State *L, Instance *self) {
		const char *key = luaL_checkstring(L, 2);

		if (key && self) {
			auto *property = self->FindProperty(key);
			if (property) {
				if (property->IsWriteImplemented()) {
					auto value = property->CheckStack(L, 3);
					property->Write(self, value);
					return 0;
				} else {
					luaL_error(L, "Property %s is read-only", key);
				}
			}
		}

		luaL_error(L, "Unknown property %s", key);

		return 0;
	};

	int Instance::LNamecall(lua_State *L, Instance *self) {
		const char *key = lua_namecallatom(L, nullptr);

		if (key && self) {
			const auto *method = self->FindMethod(key);
			if (method) {
				return method->Call(L, self);
			}
		}

		luaL_error(L, "%s is not a valid method of %s", key, self->Name[self].c_str());
		return 0;
	};
}
