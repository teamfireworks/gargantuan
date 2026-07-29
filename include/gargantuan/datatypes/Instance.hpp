#pragma once

#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/scripting/Userdata.hpp"

#include <lua.h>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#define G_INSTANCE_DECL(instanceType) static const InstanceClassDefinition CLASS_DEFINITION;

namespace gargantuan {
	class Instance;
	struct InstanceClassDefinition {
		std::string_view ClassName = "Instance";
		std::shared_ptr<Instance> (*Constructor)();
		std::optional<std::string_view> Superclass = "Instance";
		std::unordered_map<std::string_view, UserdataProperty<Instance>> Properties = {};
		std::unordered_map<std::string_view, UserdataMethod<Instance>> Methods = {};
	};

	G_SHARED_USERDATA_DECL(
		Instance,

		using Pointer = std::shared_ptr<Instance>;
		using Self = Userdata<Instance, Pointer>;
		static const gargantuan::InstanceClassDefinition CLASS_DEFINITION;

		virtual ~Instance() = default;
		void Destroy() {}

		std::string_view Name = CLASS_DEFINITION.ClassName;
		std::vector<std::shared_ptr<Instance>> Children;
		Instance *Parent = nullptr;
		void SetParent(std::shared_ptr<Instance> newParent);

		G_SIGNAL(ChildAdded, Instance::Pointer);
		G_SIGNAL(ChildRemoved, Instance::Pointer);
		G_SIGNAL(DescendantAdded, Instance::Pointer);
		G_SIGNAL(DescendantRemoved, Instance::Pointer);

		std::optional<Self::Property> FindProperty(std::string_view name);
		std::optional<Self::Method> FindMethod(std::string_view name);

		static int LIndex(lua_State *L, Instance *instance);
		static int LNewIndex(lua_State *L, Instance *instance);
		static int LNamecall(lua_State *L, Instance *instance);

		std::string GetFullName();
		bool IsA(std::string_view className);
		std::vector<std::shared_ptr<Instance>> & GetChildren();
		std::vector<std::shared_ptr<Instance>> GetDescendants();
		std::shared_ptr<Instance> FindFirstChild(std::string_view name, bool recursive = false);
		std::shared_ptr<Instance> FindFirstChildOfClass(std::string_view className);
		std::shared_ptr<Instance> FindFirstChildWhichIsA(std::string_view className);
		std::shared_ptr<Instance> FindFirstDescendant(std::string_view name);
		std::shared_ptr<Instance> FindFirstDescendantOfClass(std::string_view className);
		std::shared_ptr<Instance> FindFirstDescendantWhichIsA(std::string_view className);

		template <typename T>
		bool IsClass() const { return dynamic_cast<const T *>(this) != nullptr; }

		template <typename T>
		T *Cast() const { return dynamic_cast<const T *>(this); }

		template <typename T>
		T *Cast() { return dynamic_cast<T *>(this); }

		template <typename T>
		const T *Cast() const { return dynamic_cast<const T *>(this); }

		private : void CollectDescendants(std::vector<std::shared_ptr<Instance>> &descendants);

	);

	template <typename Subclass>
		requires std::is_base_of_v<Instance, Subclass> && (!std::is_same_v<Instance, Subclass>)
	struct StackValue<std::shared_ptr<Subclass>> {
	  public:
		static inline std::string_view ReflectedTypedef() {
			return Subclass::CLASS_DEFINITION.ClassName;
		};

		static bool Is(lua_State *L, int idx) {
			if (!StackValue<Instance::Pointer>::Is(L, idx)) return false;
			auto instance = StackValue<Instance::Pointer>::From(L, idx);
			return instance && instance->IsA(Subclass::CLASS_DEFINITION.ClassName);
		};

		static std::shared_ptr<Subclass> From(lua_State *L, int idx) {
			auto instance = gargantuan::StackValue<Instance::Pointer>::From(L, idx);
			return instance ? std::dynamic_pointer_cast<Subclass>(instance) : nullptr;
		};

		static int Push(lua_State *L, std::shared_ptr<Subclass> value) {
			return gargantuan::StackValue<Instance::Pointer>::Push(L, std::static_pointer_cast<Instance>(value));
		};
	};
} // namespace gargantuan
