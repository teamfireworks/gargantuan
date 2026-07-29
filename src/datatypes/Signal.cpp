#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/scripting/Userdata.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

#include <SDL3/SDL.h>
#include <algorithm>
#include <lua.h>
#include <lualib.h>
#include <memory>

namespace gargantuan {
	G_USERDATA_IMPL(
		SignalConnection,
		.Tag = UserdataTag::SignalConnection,
		.Type = "SignalConnection",
		.Properties = {{"Connected", Property::fromReadonlyMember<&SignalConnection::Connected>()}},
		.Methods = {
			{"Disconnect", Method::fromMember<&SignalConnection::Disconnect>()},
			{"__gc", {&SignalConnection::LGarbageCollect}}
		}
	);

	SignalConnection::SignalConnection(CallbackType callback, lua_State *L, int callbackReference)
		: Callback(std::move(callback)), L(L ? lua_mainthread(L) : nullptr), CallbackReference(callbackReference),
		  Connected(true) {}

	void SignalConnection::Disconnect() {
		if (Connected) {
			Connected = false;
			if (L && CallbackReference != LUA_NOREF && CallbackReference != LUA_REFNIL) {
				lua_unref(L, CallbackReference);
				CallbackReference = LUA_NOREF;
				L = nullptr;
			}
		}
	}

	int SignalConnection::LGarbageCollect(lua_State *L, SignalConnection *self) {
		if (self) {
			self->Disconnect();
		}
		return 0;
	}

	G_USERDATA_IMPL(
		BaseSignal,
		.Tag = UserdataTag::Signal,
		.Type = "Signal",
		.Properties = {{"Type", Property::fromRead([](BaseSignal *self) { return self->GetSignalType(); })}},
		.Methods = {
			{"Connect", Method{BaseSignal::LConnect}},
			{"Once", Method{BaseSignal::LOnce}},
			{"Wait", Method{BaseSignal::LWait}},
			{"Fire", Method{BaseSignal::LFire}},
		}
	);

	SignalConnection::Pointer
	BaseSignal::Connect(std::function<void(std::any)> callback, lua_State *L, int callbackReference) {
		auto connection = std::make_shared<SignalConnection>(callback, L, callbackReference);
		Connections.push_back(connection);
		return connection;
	};

	SignalConnection::Pointer
	BaseSignal::Once(std::function<void(std::any)> callback, lua_State *L, int callbackReference) {
		auto connection = std::make_shared<SignalConnection>(nullptr, L, callbackReference);
		std::weak_ptr<SignalConnection> weakConnection = connection;
		connection->Callback = [weakConnection, callback](CallbackArgument value) {
			if (auto conn = weakConnection.lock()) {
				conn->Disconnect();
			}

			if (callback) {
				callback(value);
			}
		};
		Connections.push_back(connection);
		return connection;
	};

	void BaseSignal::Fire(CallbackArgument value) {
		for (auto it = Connections.begin(); it != Connections.end();) {
			auto &connection = *it;
			if (!connection || !connection->Connected) {
				it = Connections.erase(it);
			} else {
				if (connection->Callback) {
					connection->Callback(value);
				}
				++it;
			}
		}
	}

	int BaseSignal::LConnect(lua_State *L, BaseSignal *signal) {
		int callbackReference = LReferenceCallback(L, 2);
		lua_State *mainState = lua_mainthread(L);

		return StackValue<SignalConnection::Pointer>::Push(
			L,
			signal->Connect(
				[mainState, callbackReference, signal](CallbackArgument value) {
					LRunCallback(mainState, signal, callbackReference, value);
				},
				mainState,
				callbackReference
			)
		);
	}

	int BaseSignal::LOnce(lua_State *L, BaseSignal *signal) {
		int callbackReference = LReferenceCallback(L, 2);
		lua_State *mainState = lua_mainthread(L);

		return StackValue<SignalConnection::Pointer>::Push(
			L,
			signal->Once(
				[mainState, callbackReference, signal](CallbackArgument value) {
					LRunCallback(mainState, signal, callbackReference, value);
				},
				mainState,
				callbackReference
			)
		);
	}

	int BaseSignal::LWait(lua_State *L, BaseSignal *signal) {
		signal->Once(
			[L, signal](CallbackArgument value) {
				int argumentCount = signal->LPushArgument(L, value);
				int status = lua_resume(L, nullptr, argumentCount);
				if (status != LUA_OK && status != LUA_YIELD) {
					SDL_Log("Failed to resume thread after signal: %s", lua_tostring(L, -1));
					lua_pop(L, 1);
				};
			},
			L,
			LUA_NOREF
		);
		return lua_yield(L, 0);
	}

	int BaseSignal::LFire(lua_State *L, BaseSignal *signal) {
		if (signal->GetSignalType() != Enums::SignalType::User) {
			luaL_error(L, "Cannot fire Signals created by the engine");
			return 0;
		}

		lua_State *mainState = lua_mainthread(L);
		auto stackCount = lua_gettop(L);
		auto argumentCount = std::max(stackCount - 1, 0);
		auto argumentVector = std::make_shared<std::vector<int>>();
		argumentVector->reserve(argumentCount);

		for (int i = 2; i <= stackCount; ++i) {
			lua_pushvalue(L, i);
			int ref = lua_ref(mainState, -1);
			lua_pop(L, 1);
			argumentVector->push_back(ref);
		}

		signal->Fire(argumentVector);
		return 0;
	}

	int UserSignal::LPushArgument(lua_State *L, std::any value) {
		if (!value.has_value()) {
			return 0;
		}

		auto argumentsPointer = std::any_cast<std::shared_ptr<std::vector<int>>>(&value);
		if (!argumentsPointer || !*argumentsPointer) {
			return 0;
		}

		lua_State *mainState = lua_mainthread(L);
		int pushedCount = 0;

		for (int ref : **argumentsPointer) {
			lua_getref(mainState, ref);

			if (L != mainState) {
				lua_xmove(mainState, L, 1);
			}

			pushedCount++;
		}

		return pushedCount;
	}

	int BaseSignal::LReferenceCallback(lua_State *L, int idx) {
		if (!lua_isfunction(L, idx)) {
			luaL_typeerror(L, idx, "function");
		}

		lua_State *mainState = lua_mainthread(L);
		lua_pushvalue(L, idx);
		if (L != mainState) {
			lua_xmove(L, mainState, 1);
		}

		int ref = lua_ref(mainState, -1);
		lua_pop(mainState, 1);

		return ref;
	}

	void BaseSignal::LRunCallback(lua_State *mainState, BaseSignal *signal, int callbackReference, std::any value) {
		if (callbackReference == LUA_NOREF || callbackReference == LUA_REFNIL) {
			return;
		}

		lua_State *thread = lua_newthread(mainState);

		lua_getref(mainState, callbackReference);
		if (!lua_isfunction(mainState, -1)) {
			lua_pop(mainState, 1);
			lua_pop(mainState, 1); // pop thread
			return;
		}

		lua_xmove(mainState, thread, 1);

		int arguments = signal->LPushArgument(thread, value);
		int status = lua_resume(thread, mainState, arguments);

		if (status != LUA_OK && status != LUA_YIELD) {
			SDL_Log("Signal error: %s", lua_tostring(thread, -1));
		}

		lua_pop(mainState, 1);
	}
} // namespace gargantuan
