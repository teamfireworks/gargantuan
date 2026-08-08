#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/scripting/Userdata.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

#include <SDL3/SDL_log.h>
#include <algorithm>
#include <cstddef>
#include <lua.h>
#include <lualib.h>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace gargantuan {
	G_USERDATA_IMPL(
		Instance,
		.Tag = UserdataTag::Instance,
		.Type = "Instance",
		.Methods = {
			{"__index", Method{&Instance::LIndex}},
			{"__newindex", Method{&Instance::LNewIndex}},
			{"__namecall", Method{&Instance::LNamecall}},
		}
	);

	G_INSTANCE_IMPL(
		Instance,
		.Superclass = std::nullopt,
		.Properties =
			{
				{"Archivable", Property::fromMember<&Instance::Name>()},
				{"Name", Property::fromMember<&Instance::Name>()},
				{
					"ClassName",
					Property::fromRead([](Instance *instance) -> std::string_view {
						return InstanceClassRegistry::GetDefinition(instance)->ClassName;
					}),
				},
				{
					"Parent",
					Property::fromReadWrite<std::shared_ptr<Instance>>(
						[](Instance *instance) -> std::optional<std::shared_ptr<Instance>> {
							return instance->Parent ? instance->Parent->shared_from_this() : nullptr;
						},
						[](Instance *instance, std::shared_ptr<Instance> newParent) { instance->SetParent(newParent); }
					),
				},
			},
		.Methods = {
			{"IsA", Method::fromMember<&Instance::IsA>()},
			{"GetFullName", Method::fromMember<&Instance::GetFullName>()},
			{"GetChildren", Method::fromMember<&Instance::GetChildren>()},
			{"GetDescendants", Method::fromMember<&Instance::GetDescendants>()},
			{"FindFirstChild", Method::fromMember<&Instance::FindFirstChild>()},
			{"FindFirstChildOfClass", Method::fromMember<&Instance::FindFirstChildOfClass>()},
			{"FindFirstChildWhichIsA", Method::fromMember<&Instance::FindFirstChildWhichIsA>()},
			{"FindFirstDescendant", Method::fromMember<&Instance::FindFirstDescendant>()},
			{"FindFirstDescendantOfClass", Method::fromMember<&Instance::FindFirstDescendantOfClass>()},
			{"FindFirstDescendantWhichIsA", Method::fromMember<&Instance::FindFirstDescendantWhichIsA>()},
			{"FindFirstAncestor", Method::fromMember<&Instance::FindFirstAncestor>()},
			{"FindFirstAncestorOfClass", Method::fromMember<&Instance::FindFirstAncestorOfClass>()},
			{"FindFirstAncestorWhichIsA", Method::fromMember<&Instance::FindFirstAncestorWhichIsA>()},
		}
	);

	void Instance::SetParent(std::shared_ptr<Instance> newParent) {
		if (Destroyed || Parent == newParent.get()) return;

		std::shared_ptr<Instance> self = shared_from_this();

		// This whole subtree leaves the old ancestry and joins the new one, so
		// collect it once up front and reuse it for both sets of signals
		std::vector<std::shared_ptr<Instance>> subtree = {self};
		CollectDescendants(subtree);

		if (Parent != nullptr) {
			auto &oldChildren = Parent->Children;
			if (auto it = std::find(oldChildren.begin(), oldChildren.end(), self); it != oldChildren.end()) {
				oldChildren.erase(it);
				Parent->ChildRemoved->Fire(self);
			}

			for (Instance *ancestor = Parent; ancestor != nullptr; ancestor = ancestor->Parent) {
				for (auto &node : subtree) {
					ancestor->DescendantRemoved->Fire(node);
				}
			}
		}

		Parent = newParent.get();

		if (newParent != nullptr) {
			newParent->Children.push_back(self);
			newParent->ChildAdded->Fire(self);

			for (Instance *ancestor = newParent.get(); ancestor != nullptr; ancestor = ancestor->Parent) {
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
		auto children = Children;
		for (auto &child : children) {
			child->Destroy();
		}
	}

	void Instance::Destroy() {
		if (Destroyed) {
			return;
		}

		Destroying->Fire({});

		auto children = Children;
		for (auto &child : children) {
			child->Destroy();
		}

		SetParent(nullptr);
		Destroyed = true;
	}

	const Instance::Self::Property *Instance::FindProperty(std::string_view name) {
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
			const auto *property = self->FindProperty(key);
			if (property) {
				if (property->Read) {
					// lua_remove(L, 1);
					// lua_remove(L, 1);
					return property->PushStack(L, property->Read(self));
				} else {
					luaL_error(L, "Property %s is write-only", key);
				}
			} else if (auto child = self->FindFirstChild(key)) {
				// lua_settop(L, 0);
				StackValue<std::shared_ptr<Instance>>::Push(L, child);
				return 1;
			}
		}

		return 0;
	};

	int Instance::LNewIndex(lua_State *L, Instance *self) {
		const char *key = luaL_checkstring(L, 2);

		if (key && self) {
			const auto *property = self->FindProperty(key);
			if (property) {
				if (property->Write) {
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

		luaL_error(L, "%s is not a valid method of %s", key, self->Name.data());
		return 0;
	};

	std::string Instance::GetFullName() {
		std::vector<std::string_view> path;

		size_t totalLength = 0;
		Instance *current = this;

		while (current) {
			auto &name = current->Name;
			path.push_back(name);
			totalLength += name.size() + 1;
			current = current->Parent;
		};

		if (path.empty()) {
			return "";
		}

		if (totalLength > 0) {
			totalLength--;
		}

		std::string fullName;
		fullName.reserve(totalLength);

		auto begin = path.rbegin();
		for (auto it = begin; it != path.rend(); ++it) {
			if (it != begin) {
				fullName.push_back('.');
			}
			fullName.append(*it);
		}

		return fullName;
	};

	bool Instance::IsA(std::string_view className) {
		auto currentDefinition = InstanceClassRegistry::GetDefinition(this);
		while (true) {
			if (currentDefinition->ClassName == className) {
				return true;
			}

			auto superclass = currentDefinition->Superclass;
			if (superclass.has_value()) {
				currentDefinition = InstanceClassRegistry::GetDefinitionByName(superclass.value());
			} else {
				return false;
			}
		}
	}

	std::vector<std::shared_ptr<Instance>> &Instance::GetChildren() {
		return Children;
	}

	void Instance::CollectDescendants(std::vector<std::shared_ptr<Instance>> &descendants) {
		for (const auto &child : Children) {
			descendants.push_back(child);
			child->CollectDescendants(descendants);
		}
	}

	std::vector<std::shared_ptr<Instance>> Instance::GetDescendants() {
		std::vector<std::shared_ptr<Instance>> descendants;
		CollectDescendants(descendants);
		return descendants;
	}

	std::shared_ptr<Instance> Instance::FindFirstChild(std::string_view name, bool recursive) {
		for (const auto &child : Children) {
			if (child->Name == name) return child;
			if (recursive) {
				if (auto found = child->FindFirstChild(name, recursive)) return found;
			}
		};
		return nullptr;
	}

	std::shared_ptr<Instance> Instance::FindFirstChildOfClass(std::string_view className, bool recursive) {
		for (const auto &child : Children) {
			if (InstanceClassRegistry::GetDefinition(child.get())->ClassName == className) return child;
			if (recursive) {
				if (auto found = child->FindFirstChildOfClass(className, recursive)) return found;
			}
		};
		return nullptr;
	}

	std::shared_ptr<Instance> Instance::FindFirstChildWhichIsA(std::string_view className, bool recursive) {
		for (const auto &child : Children) {
			if (InstanceClassRegistry::GetDefinition(child.get())->InheritedClasses.contains(className)) return child;
			if (recursive) {
				if (auto found = child->FindFirstChildWhichIsA(className, recursive)) return found;
			}
		};
		return nullptr;
	}

	std::shared_ptr<Instance> Instance::FindFirstDescendant(std::string_view name) {
		return FindFirstChild(name, true);
	}

	std::shared_ptr<Instance> Instance::FindFirstDescendantOfClass(std::string_view className) {
		return FindFirstChildOfClass(className, true);
	}

	std::shared_ptr<Instance> Instance::FindFirstDescendantWhichIsA(std::string_view className) {
		return FindFirstChildWhichIsA(className, true);
	}

	std::shared_ptr<Instance> Instance::FindFirstAncestor(std::string_view name) {
		auto *current = this->Parent;
		while (current) {
			if (current->Name == name) return current->shared_from_this();
			current = current->Parent;
		}
		return nullptr;
	}

	std::shared_ptr<Instance> Instance::FindFirstAncestorOfClass(std::string_view className) {
		auto *current = this->Parent;
		while (current) {
			if (InstanceClassRegistry::GetDefinition(current)->ClassName == className) {
				return current->shared_from_this();
			}
			current = current->Parent;
		}
		return nullptr;
	}

	std::shared_ptr<Instance> Instance::FindFirstAncestorWhichIsA(std::string_view className) {
		auto *current = this->Parent;
		while (current) {
			if (InstanceClassRegistry::GetDefinition(current)->InheritedClasses.contains(className)) {
				return current->shared_from_this();
			}
			current = current->Parent;
		}
		return nullptr;
	}
}
