#pragma once

#include "gargantuan/reflection/Enums.hpp"
#include "gargantuan/scripting/StackValue.hpp"
#include <functional>
#include <string>
#include <utility>

namespace gargantuan {
	#ifdef __NDEBUG__
		#define DEBUG_ENUM_ENTRY Debug = 0
	#else
		#define DEBUG_ENUM_ENTRY Debug
	#endif

	G_ENUM(
		Permission,

		// Minimum of any script
		None,
		// Minimum of any plugin
		Plugin,
		// Minimum of the local development machine, unused in a shipped game
		LocalUser,

		// We skip over WritePlayer since it is irrelevant to Gargantuan;
		// games itself make player instances

		// Minimum of engine-level scripts
		EngineScript = 4,
		// Minimum of Gargantuan itself
		Engine,

		DEBUG_ENUM_ENTRY,

		// This feature should never be used
		Never = 7
	);

	#undef DEBUG_ENUM_ENTRY

	class Instance;

	class InstanceProperty {
	  private:
		template <typename T> struct MemberPointerTraits;

		template <typename Class, typename T> struct MemberPointerTraits<T Class::*> {
			using ClassType = Class;
			using MemberType = T;
			using ArgsType = std::tuple<>;
		};

		template <typename Class, typename Return, typename... Args>
		struct MemberPointerTraits<Return (Class::*)(Args...)> {
			using ClassType = Class;
			using ReturnType = Return;
			using MemberType = Return;
			using ArgsType = std::tuple<Args...>;
		};

		template <typename Class, typename Return, typename... Args>
		struct MemberPointerTraits<Return (Class::*)(Args...) const> {
			using ClassType = Class;
			using ReturnType = Return;
			using MemberType = Return;
			using ArgsType = std::tuple<Args...>;
		};

	  public:
		std::string Name{};
		std::string ReflectedTypedef{};
		std::any Unmodified{};
		bool Signal{false};
		bool Serializable{false};

		Enums::Permission ReadPermission = Enums::Permission::None;
		std::function<std::any(Instance *self)> Read;
		std::function<int(lua_State *L, std::any value)> PushStack;

		Enums::Permission WritePermission = Enums::Permission::None;
		std::function<void(Instance *self, std::any value)> Write;
		std::function<bool(lua_State *L, int idx)> IsStack;
		std::function<std::any(lua_State *L, int idx)> FromStack;

		explicit InstanceProperty(std::string name) : Name(std::move(name)) {};

		InstanceProperty &SetReflectedTypedef(std::string type) {
			ReflectedTypedef = type;
			return *this;
		}

		InstanceProperty &SetUnmodified(std::any unmodified) {
			Unmodified = unmodified;
			return *this;
		}

		InstanceProperty &SetSignal(bool isSignal = true) {
			Signal = isSignal;
			return *this;
		}

		InstanceProperty &SetSerializable(bool serializable = true) {
			Serializable = serializable;
			return *this;
		}

		InstanceProperty &SetReadPermission(Enums::Permission permission) {
			ReadPermission = permission;
			return *this;
		}

		InstanceProperty &SetWritePermission(Enums::Permission permission) {
			WritePermission = permission;
			return *this;
		}

		std::pair<std::string, InstanceProperty> IntoPair() && {
			std::string propName = std::move(Name);
			InstanceProperty prop = std::move(*this);
			prop.Name = propName;
			return {propName, std::move(prop)};
		}

		std::pair<std::string, InstanceProperty> IntoPair() & {
			std::string propName = std::move(Name);
			InstanceProperty prop = std::move(*this);
			prop.Name = propName;
			return {propName, std::move(prop)};
		}

		template <auto Pointer> InstanceProperty &UseRead() {
			using Traits = MemberPointerTraits<decltype(Pointer)>;
			using ClassType = typename Traits::ClassType;
			using MemberType = typename Traits::MemberType;

			if constexpr (std::is_member_function_pointer_v<decltype(Pointer)>) {
				Read = [](Instance *self) -> std::any {
					ClassType *obj = reinterpret_cast<ClassType *>(self);
					return (obj->*Pointer)();
				};
			} else if constexpr (std::is_member_object_pointer_v<decltype(Pointer)>) {
				Read = [](Instance *self) -> std::any {
					ClassType *obj = reinterpret_cast<ClassType *>(self);
					return obj->*Pointer;
				};
			}

			PushStack = [](lua_State *L, const std::any &value) -> int {
				if (auto val = std::any_cast<MemberType>(&value)) return StackValue<MemberType>::Push(L, *val);
				return 0;
			};

			return *this;
		};

		template <auto Pointer> InstanceProperty &UseWrite() {
			using Traits = MemberPointerTraits<decltype(Pointer)>;
			using ClassType = typename Traits::ClassType;

			if constexpr (std::is_member_function_pointer_v<decltype(Pointer)>) {
				using RawArgType = std::tuple_element_t<0, typename Traits::ArgsType>;
				using ArgType = std::decay_t<RawArgType>;

				Write = [](Instance *self, const std::any &value) {
					ClassType *obj = reinterpret_cast<ClassType *>(self);
					if (auto val = std::any_cast<ArgType>(&value)) {
						(obj->*Pointer)(*val);
					}
				};

				IsStack = [](lua_State *L, int idx) { return StackValue<ArgType>::Is(L, idx); };
				FromStack = [](lua_State *L, int idx) { return StackValue<ArgType>::From(L, idx); };
			} else {
				using MemberType = typename Traits::MemberType;

				Write = [](Instance *self, const std::any &value) {
					ClassType *obj = reinterpret_cast<ClassType *>(self);
					if (auto val = std::any_cast<MemberType>(&value)) {
						obj->*Pointer = *val;
					}
				};

				IsStack = [](lua_State *L, int idx) { return StackValue<MemberType>::Is(L, idx); };
				FromStack = [](lua_State *L, int idx) { return StackValue<MemberType>::From(L, idx); };
			}

			return *this;
		};
	};
}
