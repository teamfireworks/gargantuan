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
				{"Name", Property::fromMember<&Instance::Name>(true, true)},
				{
					"ClassName",
					Property::fromRead([](Instance *instance) -> std::string_view {
						return InstanceClassRegistry::GetDefinition(instance)->ClassName;
					}),
				},
				{
					"Parent",
					Property::fromReadWrite<Instance::Pointer>(
						[](Instance *instance) -> std::optional<Instance::Pointer> {
							return instance->Parent ? instance->Parent->shared_from_this() : nullptr;
						},
						[](Instance *instance, Instance::Pointer newParent) { instance->SetParent(newParent); }
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
		}
	);

	// TODO: fire DescendantAdded/Removed signals
	void Instance::SetParent(std::shared_ptr<Instance> newParent) {
		std::shared_ptr<Instance> self = shared_from_this();

		if (Parent != nullptr) {
			auto &oldChildren = Parent->Children;
			if (auto it = std::find(oldChildren.begin(), oldChildren.end(), self); it != oldChildren.end()) {
				oldChildren.erase(it);
				Parent->ChildRemoved->Fire(self);
			}
		}

		Parent = newParent.get();

		if (newParent != nullptr) {
			newParent->Children.push_back(self);
			newParent->ChildAdded->Fire(self);
		}
	}

	std::optional<Instance::Userdata::Property> Instance::FindProperty(std::string_view name) {
		auto currentDefinition = InstanceClassRegistry::GetDefinition(this);
		while (currentDefinition) {
			if (auto it = currentDefinition->Properties.find(name); it != currentDefinition->Properties.end()) {
				return it->second;
			}

			auto superclass = currentDefinition->Superclass;
			if (superclass.has_value()) {
				currentDefinition = InstanceClassRegistry::GetDefinitionByName(superclass.value());
				continue;
			} else {
				return {};
			}
		}
		return {};
	}

	std::optional<Instance::Userdata::Method> Instance::FindMethod(std::string_view name) {
		auto currentDefinition = InstanceClassRegistry::GetDefinition(this);
		while (currentDefinition) {
			if (auto it = currentDefinition->Methods.find(name); it != currentDefinition->Methods.end()) {
				return it->second;
			}

			auto superclass = currentDefinition->Superclass;
			if (superclass.has_value()) {
				currentDefinition = InstanceClassRegistry::GetDefinitionByName(superclass.value());
				continue;
			} else {
				return {};
			}
		}
		return {};
	}

	int Instance::LIndex(lua_State *L, Instance *self) {
		const char *key = luaL_checkstring(L, 2);

		if (key && self) {
			auto property = self->FindProperty(key);
			if (property.has_value()) {
				if (property->Read) {
					// lua_remove(L, 1);
					// lua_remove(L, 1);
					return property->PushStack(L, property->Read(self));
				} else {
					luaL_error(L, "Property %s is write-only", key);
				}
			} else if (auto child = self->FindFirstChild(key)) {
				// lua_settop(L, 0);
				StackValue<Instance::Pointer>::Push(L, child);
				return 1;
			}
		}

		return 0;
	};

	int Instance::LNewIndex(lua_State *L, Instance *self) {
		const char *key = luaL_checkstring(L, 2);

		if (key && self) {
			auto property = self->FindProperty(key);
			if (property.has_value()) {
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
			auto method = self->FindMethod(key);
			if (method.has_value()) {
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
			if (child->Name == name) {
				return child;
			}
		};
		return nullptr;
	}

	std::shared_ptr<Instance> Instance::FindFirstChildOfClass(std::string_view className) {
		for (const auto &child : Children) {
			if (InstanceClassRegistry::GetDefinition(child.get())->ClassName == className) {
				return child;
			}
		};
		return nullptr;
	}
}
