#pragma once

#include "gargantuan.runtime/StackValue.hpp"
#include "gargantuan.runtime/Userdata.hpp"
#include "gargantuan/reflection/Enums.hpp"

#include <SDL3/SDL_log.h>
#include <any>
#include <cstddef>
#include <functional>
#include <lua.h>
#include <lualib.h>
#include <memory>
#include <vector>

#define G_SIGNAL(propertyName, signalType)                                                                             \
	Signal<signalType>::Pointer propertyName = std::make_shared<Signal<signalType>>();

namespace gargantuan {
	G_ENUM(SignalType, Engine, User);

	G_SHARED_USERDATA_DECL(
		SignalConnection, typedef std::shared_ptr<SignalConnection> Pointer;
		typedef Userdata<SignalConnection, Pointer> Self;

		typedef std::any CallbackArgument;
		typedef std::function<void(CallbackArgument)> CallbackType;

		SignalConnection(CallbackType callback, lua_State *L = nullptr, int callbackReference = LUA_NOREF);

		CallbackType Callback;
		lua_State * L;
		int CallbackReference;
		bool Connected = true;

		void Disconnect();

		static int LGarbageCollect(lua_State *L, SignalConnection *signal);
	);

	G_SHARED_USERDATA_DECL(
		BaseSignal,

		typedef std::shared_ptr<BaseSignal> Pointer;
		typedef Userdata<BaseSignal, Pointer> Self;

		typedef std::any CallbackArgument;
		typedef std::function<void(CallbackArgument)> CallbackType;

		std::vector<SignalConnection::Pointer> Connections;
		int FiringDepth = 0;

		virtual SignalConnection::Pointer Connect(
			CallbackType callback, lua_State *L = nullptr, int callbackReference = LUA_NOREF
		);

		virtual SignalConnection::Pointer Once(
			CallbackType callback, lua_State *L = nullptr, int callbackReference = LUA_NOREF
		);

		void Fire(CallbackArgument argument);

		virtual int LPushArgument(lua_State *L, CallbackArgument value) = 0;

		static int LConnect(lua_State *L, BaseSignal *signal);
		static int LOnce(lua_State *L, BaseSignal *signal);
		static int LWait(lua_State *L, BaseSignal *signal);
		static int LFire(lua_State *L, BaseSignal *signal);

		static int LReferenceCallback(lua_State *L, int idx);
		static void LRunCallback(lua_State *mainState, BaseSignal *signal, int callbackReference, std::any value);

		virtual Enums::SignalType GetSignalType() = 0;
	);

	template <typename T> struct Signal : BaseSignal {
		typedef std::shared_ptr<Signal> Pointer;
		typedef Userdata<Signal, Pointer> This;

		typedef T TypedCallbackArgument;
		typedef std::function<void(T)> TypedCallback;

		Enums::SignalType GetSignalType() override {
			return Enums::SignalType::Engine;
		}

		SignalConnection::Pointer
		Connect(TypedCallback callback, lua_State *L = nullptr, int callbackReference = LUA_NOREF) {
			return BaseSignal::Connect(
				[callback](std::any value) {
					if (value.has_value() && value.type() == typeid(T)) {
						callback(std::any_cast<T>(value));
					}
				},
				L,
				callbackReference
			);
		}

		SignalConnection::Pointer
		Once(TypedCallback callback, lua_State *L = nullptr, int callbackReference = LUA_NOREF) {
			return BaseSignal::Once(
				[callback](std::any value) {
					if (value.has_value() && value.type() == typeid(T)) {
						callback(std::any_cast<T>(value));
					}
				},
				L,
				callbackReference
			);
		}

		void Fire(T argument) {
			BaseSignal::Fire(std::any(argument));
		}

		int LPushArgument(lua_State *L, std::any value) override {
			if (!value.has_value()) {
				return 0;
			}
			try {
				if (value.type() == typeid(T)) {
					return StackValue<T>::Push(L, std::any_cast<T>(value));
				}
			} catch (const std::bad_any_cast &e) {
				SDL_Log("Signal LPushArgument cast error: %s", e.what());
			}
			return 0;
		};
	};

	struct UserSignal : BaseSignal {
		typedef std::shared_ptr<UserSignal> Pointer;
		typedef Userdata<UserSignal, Pointer> This;

		Enums::SignalType GetSignalType() override {
			return Enums::SignalType::User;
		}

		int LPushArgument(lua_State *L, std::any value) override;
	};

	template <typename Subclass>
		requires std::is_base_of_v<BaseSignal, Subclass> && (!std::is_same_v<BaseSignal, Subclass>)
	struct StackValue<std::shared_ptr<Subclass>> {
	  public:
		static inline std::string_view ReflectedTypedef() {
			return StackValue<BaseSignal::Pointer>::ReflectedTypedef();
		};

		static bool Is(lua_State *L, int idx) {
			return StackValue<BaseSignal::Pointer>::Is(L, idx);
		};

		static std::shared_ptr<Subclass> From(lua_State *L, int idx) {
			auto instance = gargantuan::StackValue<BaseSignal::Pointer>::From(L, idx);
			return instance ? std::dynamic_pointer_cast<Subclass>(instance) : nullptr;
		};

		static int Push(lua_State *L, std::shared_ptr<Subclass> value) {
			return gargantuan::StackValue<BaseSignal::Pointer>::Push(L, std::static_pointer_cast<BaseSignal>(value));
		};
	};
}
