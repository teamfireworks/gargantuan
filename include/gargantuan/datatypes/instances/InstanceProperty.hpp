#pragma once

#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/reflection/Enums.hpp"
#include "gargantuan/scripting/StackValue.hpp"
#include <any>
#include <cassert>
#include <cstddef>
#include <lua.h>

#include <functional>
#include <lualib.h>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace gargantuan {
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

#ifdef __NDEBUG__
		Debug = 0
#else
		Debug
#endif
	);

	class Instance;

	template <typename Type = nullptr_t> class InstanceProperty {
	  public:
		typedef std::unordered_map<std::string_view, InstanceProperty<std::any>> RegisteredMap;

		bool Serializable = false;
		std::string Name;

		Enums::Permission ReadPermission = Enums::Permission::None;
		std::function<std::string()> GetReadType;
		std::function<Type(Instance *self)> RawRead;
		std::function<int(lua_State *L, Type value)> PushStack;

		Enums::Permission WritePermission = Enums::Permission::None;
		std::function<std::string()> GetWriteType;
		std::function<void(Instance *self, Type value)> RawWrite;
		std::function<bool(lua_State *L, int idx)> IsStack;
		std::function<Type(lua_State *L, int idx)> FromStack;

		Signal<Type *> OnWrite = std::make_shared<Signal<Type>>();

		const bool IsReadImplemented() const {
			return RawRead != nullptr;
		}

		const bool IsWriteImplemented() const {
			return RawWrite != nullptr;
		}

		Type CheckStack(lua_State *L, int idx) {
			assert(IsWriteImplemented());
			if (IsStack(L, idx)) luaL_typeerrorL(L, idx, GetWriteType().c_str());
			return FromStack(L, idx);
		}

		Type Read(Instance *self) {
			assert(IsReadImplemented());
			return RawRead(self);
		}

		void Write(Instance *self, Type value) {
			assert(IsWriteImplemented());
			RawWrite(self, value);
			OnWrite.Fire(&value);
		}

		Type operator[](Instance *self) {
			return Read(self);
		}
	};

	enum InstancePropertyPreset {
		Unimplemented,
		Value,
	};

	template <typename Type> struct CreateInstancePropertyInfo {
		std::string Name;
		InstancePropertyPreset Preset = InstancePropertyPreset::Unimplemented;

		Type Default;
		std::function<Type(Instance *self)> GetDefault = nullptr;
		bool Serializable = false;
		Enums::Permission ReadPermission = Enums::Permission::None;
		Enums::Permission WritePermission = Enums::Permission::None;
	};

	template <typename Type>
	static InstanceProperty<Type> CreateInstanceProperty(const CreateInstancePropertyInfo<Type> &info) {
		static std::unordered_map<Instance *, Type> INSTANCE_TO_VALUE;

		decltype(InstanceProperty<Type>::Read) read = nullptr;
		decltype(InstanceProperty<Type>::Write) write = nullptr;

		switch (info.Preset) {
		case InstancePropertyPreset::Value:
			read = [info](Instance *self) -> Type {
				return &INSTANCE_TO_VALUE.contains(self) ? &INSTANCE_TO_VALUE.at(self)
					   : info.GetDefault				 ? info.GetDefault(self)
														 : info.Default;
			};

			write = [](Instance *self, Type value) { &INSTANCE_TO_VALUE.emplace(self, value); };

			break;
		}

		return {
			.Name = info.Name,
			.Serializable = info.Serializable,

			.ReadPermission = info.ReadPermission,
			.GetReadType = []() -> std::string { return StackValue<Type>::GetReflectedTypedef(); },
			.RawRead = read,
			.PushStack = [info](lua_State *L, Type value) -> int { return StackValue<Type>::Push(L, value); },

			.WritePermission = info.WritePermission,
			.GetWriteType = []() -> std::string { return StackValue<Type>::GetReflectedTypedef(); },
			.RawWrite = write,
			.IsStack = [](lua_State *L, int idx) -> bool { return StackValue<Type>::Is(L, idx); },
			.FromStack = [](lua_State *L, int idx) -> Type { return StackValue<Type>::From(L, idx); },
		};
	};
}
