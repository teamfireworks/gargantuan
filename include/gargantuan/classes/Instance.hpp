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
	class Instance : public std::enable_shared_from_this<Instance>,
					 public Userdata<Instance, std::shared_ptr<Instance>>,
					 public __CODEGEN__Instance {
	  public:
		static const Definition DEFINITION;

		virtual ~Instance() = default;

		std::vector<std::shared_ptr<Instance>> Children;
		std::unordered_map<std::string, Signal<std::monostate>> PropertyChangedSignals;
		Instance *ParentPointer = nullptr;
		InstanceClassDefinition *CachedDefinition = nullptr;

		const InstanceProperty *FindProperty(std::string_view name);
		const Self::Method *FindMethod(std::string_view name);
		static int LIndex(lua_State *L, Instance *instance);
		static int LNewIndex(lua_State *L, Instance *instance);
		static int LNamecall(lua_State *L, Instance *instance);

		template <typename T> [[deprecated("dynamic_cast it urself dummy")]] bool IsClass() const {
			return dynamic_cast<const T *>(this) != nullptr;
		}
		template <typename T> [[deprecated("dynamic_cast it urself dummy")]] T *Cast() const {
			return dynamic_cast<const T *>(this);
		}
		template <typename T> [[deprecated("dynamic_cast it urself dummy")]] T *Cast() {
			return dynamic_cast<T *>(this);
		}
		template <typename T> [[deprecated("dynamic_cast it urself dummy")]] const T *Cast() const {
			return dynamic_cast<const T *>(this);
		}

		void CollectDescendants(std::vector<std::shared_ptr<Instance>> &descendants);
		void FireAncestryChanged(std::shared_ptr<Instance> child, std::shared_ptr<Instance> parent);
		void AssertIsAlive();
	};

	template <> struct StackValue<std::shared_ptr<Instance>> {
		typedef Userdata<Instance, std::shared_ptr<Instance>> This;
		static inline std::string_view ReflectedTypedef() {
			return StackValue<This>::ReflectedTypedef();
		}
		static bool Is(lua_State *L, int idx) {
			return StackValue<This>::Is(L, idx);
		}
		static std::shared_ptr<Instance> From(lua_State *L, int idx) {
			return StackValue<This>::From(L, idx);
		}
		static int Push(lua_State *L, std::shared_ptr<Instance> value) {
			return StackValue<This>::Push(L, value);
		}
	};

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
