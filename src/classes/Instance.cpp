#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/InstanceProperty.hpp"
#include "gargantuan/Log.hpp"
#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/scripting/Userdata.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

#include <lua.h>
#include <lualib.h>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
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

	void Instance::AssertIsAlive() const {
		if (Destroyed) throw std::runtime_error("Instance is destroyed");
	}

	std::string Instance::GetClassName() const {
		AssertIsAlive();
		auto wtf = *this;
		InstanceClassDefinition *definition = InstanceClassRegistry::GetDefinition(&wtf);
		return definition->ClassName;
	}

	void Instance::FireAncestryChanged(std::shared_ptr<Instance> child, std::shared_ptr<Instance> parent) {
		AncestryChanged->Fire({child, parent});
		// GetPropertyChangedSignal("Parent")->Fire({});
		for (auto &descendant : Children) {
			descendant->FireAncestryChanged(child, parent);
		}
	}

	std::optional<std::shared_ptr<Instance>> Instance::GetParent() const {
		return ParentPointer != nullptr ? std::optional(ParentPointer->shared_from_this()) : std::nullopt;
	}

	void Instance::SetParent(std::optional<std::shared_ptr<Instance>> value) {
		AssertIsAlive();

		std::shared_ptr<Instance> newParent = value.has_value() ? value.value() : nullptr;
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

		ParentPointer = newParent.get();

		// LOG_DEBUG(App, "Updating parent for %s", GetFullName().c_str());

		//  vvv this block is causing some bullshit ???
		// im gonna look at spookexes branch he fixed this somehow wait
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
		// LOG_DEBUG(App, "Successfully set parent");
	}

	void Instance::ClearAllChildren() {
		auto children = Children;
		for (auto &child : children) {
			child->Destroy();
		}
	}

	bool Instance::IsPropertyModified(std::string propertyName) {
		return true;
		// auto property = FindProperty(propertyName);

		// if (!property) throw std::runtime_error("Property does not exist");
		// if (property->Signal) throw std::runtime_error("Property is a signal");
		// if (!property->Read || property->ReadPermission == Enums::Permission::Never) {
		// 	throw std::runtime_error("Property is read-only");
		// };

		// return property->Read(this) != property->Unmodified;
	};

	std::shared_ptr<Signal<std::monostate>> Instance::GetPropertyChangedSignal(std::string propertyName) {
		if (PropertyChangedSignals.contains(propertyName)) return PropertyChangedSignals[propertyName];

		auto property = FindProperty(propertyName);

		if (!property) throw std::runtime_error("Property does not exist");
		if (!property->Write || property->WritePermission == Enums::Permission::Never) {
			throw std::runtime_error("Property is read-only");
		};

		auto signal = std::make_shared<Signal<std::monostate>>();
		PropertyChangedSignals.emplace(propertyName, signal);
		return signal;
	};

	void Instance::ResetPropertyToDefault(std::string propertyName) {
		auto property = FindProperty(propertyName);

		if (!property) throw std::runtime_error("Property does not exist");
		if (property->Signal) throw std::runtime_error("Property is a signal");
		if (!property->Write || property->WritePermission == Enums::Permission::Never) {
			throw std::runtime_error("Property is read-only");
		};

		property->Write(this, property->Unmodified);
	};

	const InstanceProperty *Instance::FindProperty(std::string name) {
		const InstanceClassDefinition *definition = InstanceClassRegistry::GetDefinition(this);
		if (!definition) return nullptr;

		auto it = definition->AllProperties.find(name);
		return it != definition->AllProperties.end() ? it->second : nullptr;
	}

	const Instance::Self::Method *Instance::FindMethod(std::string name) {
		const InstanceClassDefinition *definition = InstanceClassRegistry::GetDefinition(this);
		if (!definition) return nullptr;

		auto it = definition->AllMethods.find(name);
		return it != definition->AllMethods.end() ? it->second : nullptr;
	}

	int Instance::LIndex(lua_State *L, Instance *self) {
		const char *key = luaL_checkstring(L, 2);

		if (key && self) {
			const auto *property = self->FindProperty(key);
			if (property) {
				if (property->Read) {
					return property->PushStack(L, property->Read(self));
				} else {
					luaL_error(L, "Property %s is write-only", key);
				}
			} else if (auto child = self->FindFirstChild(key, std::nullopt)) {
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
					if (!property->IsStack(L, 3)) luaL_typeerrorL(L, 3, property->ReflectedTypedef.c_str());
					auto value = property->FromStack(L, 3);
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
			current = current->ParentPointer;
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

	bool Instance::IsA(std::string className) {
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

	std::vector<std::shared_ptr<Instance>> Instance::GetChildren() {
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

	std::shared_ptr<Instance> Instance::FindFirstChild(std::string name, std::optional<bool> recursive) {
		for (const auto &child : Children) {
			if (child->Name == name) return child;
			if (recursive) {
				if (auto found = child->FindFirstChild(name, recursive)) return found;
			}
		};
		return nullptr;
	}

	std::shared_ptr<Instance> Instance::FindFirstChildOfClass(std::string className, std::optional<bool> recursive) {
		for (const auto &child : Children) {
			if (InstanceClassRegistry::GetDefinition(child.get())->ClassName == className) return child;
			if (recursive) {
				if (auto found = child->FindFirstChildOfClass(className, recursive)) return found;
			}
		};
		return nullptr;
	}

	std::shared_ptr<Instance> Instance::FindFirstChildWhichIsA(std::string className, std::optional<bool> recursive) {
		for (const auto &child : Children) {
			if (InstanceClassRegistry::GetDefinition(child.get())->InheritedClasses.contains(className)) return child;
			if (recursive) {
				if (auto found = child->FindFirstChildWhichIsA(className, recursive)) return found;
			}
		};
		return nullptr;
	}

	std::shared_ptr<Instance> Instance::FindFirstDescendant(std::string name) {
		return FindFirstChild(name, true);
	}

	std::shared_ptr<Instance> Instance::FindFirstDescendantOfClass(std::string className) {
		return FindFirstChildOfClass(className, true);
	}

	std::shared_ptr<Instance> Instance::FindFirstDescendantWhichIsA(std::string className) {
		return FindFirstChildWhichIsA(className, true);
	}

	std::shared_ptr<Instance> Instance::FindFirstAncestor(std::string name) {
		auto *current = this->ParentPointer;
		while (current) {
			if (current->Name == name) return current->shared_from_this();
			current = current->ParentPointer;
		}
		return nullptr;
	}

	std::shared_ptr<Instance> Instance::FindFirstAncestorOfClass(std::string className) {
		auto *current = this->ParentPointer;
		while (current) {
			if (InstanceClassRegistry::GetDefinition(current)->ClassName == className) {
				return current->shared_from_this();
			}
			current = current->ParentPointer;
		}
		return nullptr;
	}

	std::shared_ptr<Instance> Instance::FindFirstAncestorWhichIsA(std::string className) {
		auto *current = this->ParentPointer;
		while (current) {
			if (InstanceClassRegistry::GetDefinition(current)->InheritedClasses.contains(className)) {
				return current->shared_from_this();
			}
			current = current->ParentPointer;
		}
		return nullptr;
	}
}
