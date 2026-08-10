#pragma once

#include "gargantuan/InstanceClassDefinition.hpp"
#include "gargantuan/classes/generated/Instance.hpp"
#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/scripting/Userdata.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

namespace gargantuan {
	G_SHARED_USERDATA_DECL(
		Instance, I_Instance;

		virtual ~Instance() = default;

		std::vector<std::shared_ptr<Instance>> Children;
		std::unordered_map<std::string, std::shared_ptr<Signal<std::monostate>>> PropertyChangedSignals;
		Instance *ParentPointer = nullptr;
		InstanceClassDefinition *CachedDefinition = nullptr;

		const InstanceProperty *FindProperty(std::string name);
		const Self::Method *FindMethod(std::string name);
		static int LIndex(lua_State *L, Instance *instance);
		static int LNewIndex(lua_State *L, Instance *instance);
		static int LNamecall(lua_State *L, Instance *instance);

		typedef std::function<void(std::shared_ptr<gargantuan::Instance> instance)> BindCallback;
		std::function<void()> BindChildren(BindCallback callback);
		std::function<void()> BindDescendants(BindCallback callback);

		void CollectDescendants(std::vector<std::shared_ptr<Instance>> &descendants);
		void FireAncestryChanged(std::shared_ptr<Instance> child, std::shared_ptr<Instance> parent);
		void AssertIsAlive() const;
	);

	template <typename Subclass>
		requires std::is_base_of_v<Instance, Subclass> && (!std::is_same_v<Instance, Subclass>)
	struct StackValue<std::shared_ptr<Subclass>> {
	  public:
		static inline std::string_view ReflectedTypedef() {
			return Subclass::CLASS_DEFINITION.ClassName;
		};

		static bool Is(lua_State *L, int idx) {
			if (!StackValue<std::shared_ptr<Instance>>::Is(L, idx)) return false;
			auto instance = StackValue<std::shared_ptr<Instance>>::From(L, idx);
			return instance && instance->IsA(Subclass::CLASS_DEFINITION.ClassName);
		};

		static std::shared_ptr<Subclass> From(lua_State *L, int idx) {
			auto instance = gargantuan::StackValue<std::shared_ptr<Instance>>::From(L, idx);
			return instance ? std::dynamic_pointer_cast<Subclass>(instance) : nullptr;
		};

		static int Push(lua_State *L, std::shared_ptr<Subclass> value) {
			return gargantuan::StackValue<std::shared_ptr<Instance>>::Push(
				L, std::static_pointer_cast<Instance>(value)
			);
		};
	};
}
