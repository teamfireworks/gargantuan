// TODO: This file was brought over from Spookes branch, please clean it up of
// AI claude slop

#include "gargantuan.core/datatypes/Signal.hpp"
#include "gargantuan.runtime/StackGuard.hpp"
#include "gargantuan.runtime/Userdata.hpp"
#include "gargantuan.runtime/UserdataTag.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
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
		Connected = false;
		if (L && CallbackReference != LUA_NOREF && CallbackReference != LUA_REFNIL) {
			lua_unref(L, CallbackReference);
			CallbackReference = LUA_NOREF;
			L = nullptr;
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
			auto conn = weakConnection.lock();

			// Mark it spent before running so a re-entrant fire cannot call it
			// twice, but hold onto the Luau reference until the call is done --
			// Disconnect releases it, and the callback still needs it
			if (conn) {
				conn->Connected = false;
			}

			if (callback) {
				callback(value);
			}

			if (conn) {
				conn->Disconnect();
			}
		};
		Connections.push_back(connection);
		return connection;
	};

	void BaseSignal::Fire(CallbackArgument value) {
		// A handler may connect, disconnect or fire this signal again, so run
		// over a snapshot rather than the live list. The snapshot holds owning
		// pointers, so a handler that clears Connections outright -- which
		// Instance::Destroy does -- cannot pull entries out from under the loop.
		auto connections = Connections;
		FiringDepth++;

		for (auto &connection : connections) {
			if (connection && connection->Connected && connection->Callback) {
				connection->Callback(value);
			}
		}

		FiringDepth--;
		if (FiringDepth == 0) {
			std::erase_if(Connections, [](const SignalConnection::Pointer &connection) {
				return !connection || !connection->Connected;
			});
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
		lua_State *mainState = lua_mainthread(L);
		if (L == mainState) {
			luaL_error(L, "Cannot Wait outside of a thread");
			return 0;
		}

		lua_pushthread(L);
		int threadReference = lua_ref(L, -1);
		lua_pop(L, 1);

		signal->Once(
			[L, mainState, threadReference, signal](CallbackArgument value) {
				int argumentCount = signal->LPushArgument(L, value);
				int status = lua_resume(L, mainState, argumentCount);
				if (status != LUA_OK && status != LUA_YIELD) {
					SDL_Log("Failed to resume thread after signal: %s", lua_tostring(L, -1));
					lua_pop(L, 1);
				};

				lua_unref(mainState, threadReference);
			},
			L,
			LUA_NOREF
		);
		return lua_yield(L, 0);
	}

	int BaseSignal::LFire(lua_State *L, BaseSignal *signal) {
		// TODO: This should be on a per-signal basis, ie. you might wanna fire
		// RunService.PreRender on the server or smshit
		if (signal->GetSignalType() != Enums::SignalType::User) {
			luaL_error(L, "Cannot fire Signals created by the engine");
			return 0;
		}

		auto stackCount = lua_gettop(L);
		auto argumentCount = std::max(stackCount - 1, 0);
		EnsureStackSpace(L, 1);

		auto argumentVector = std::make_shared<std::vector<int>>();
		argumentVector->reserve(argumentCount);

		for (int i = 2; i <= stackCount; ++i) {
			argumentVector->push_back(lua_ref(L, i));
		}

		signal->Fire(argumentVector);

		for (int ref : *argumentVector) {
			lua_unref(L, ref);
		}

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

		auto &arguments = **argumentsPointer;
		int slots = (int)arguments.size();
		if (!TryEnsureStackSpace(L, slots + 1)) {
			SDL_Log("Dropping a signal firing with %d arguments: the Luau stack cannot grow that far", slots);
			return 0;
		}

		int pushedCount = 0;
		for (int ref : arguments) {
			lua_getref(L, ref);
			pushedCount++;
		}

		return pushedCount;
	}

	int BaseSignal::LReferenceCallback(lua_State *L, int idx) {
		if (!lua_isfunction(L, idx)) {
			luaL_typeerror(L, idx, "function");
		}

		return lua_ref(L, idx);
	}

	void BaseSignal::LRunCallback(lua_State *mainState, BaseSignal *signal, int callbackReference, std::any value) {
		if (callbackReference == LUA_NOREF || callbackReference == LUA_REFNIL) {
			return;
		}

		int stackTop = lua_gettop(mainState);
		lua_State *thread = lua_newthread(mainState);

		int threadReference = lua_ref(mainState, -1);
		lua_pop(mainState, 1);

		lua_getref(mainState, callbackReference);
		if (!lua_isfunction(mainState, -1)) {
			lua_pop(mainState, 1);
			lua_unref(mainState, threadReference);
			return;
		}

		lua_xmove(mainState, thread, 1);

		// from spook
		// std::string label;
		// if (G_PROFILING) {
		// 	lua_Debug info;
		// 	if (lua_getinfo(thread, -1, "s", &info) && info.short_src) {
		// 		label = info.short_src;

		// 		// Luau reports a chunk loaded from a buffer as [string "Name"],
		// 		// which is three quarters punctuation in a chart row that is
		// 		// already short of width
		// 		constexpr std::string_view WRAPPER = "[string \"";
		// 		if (label.starts_with(WRAPPER) && label.ends_with("\"]")) {
		// 			label = label.substr(WRAPPER.size(), label.size() - WRAPPER.size() - 2);
		// 		}
		// 	} else {
		// 		label = "?";
		// 	}
		// }
		// G_PROFILE_NAMED("Script", label.data(), label.size());

		int arguments = signal->LPushArgument(thread, value);
		int status = lua_resume(thread, mainState, arguments);

		if (status != LUA_OK && status != LUA_YIELD) {
			SDL_Log("Signal error: %s", lua_tostring(thread, -1));
		}

		lua_unref(mainState, threadReference);
		lua_settop(mainState, stackTop);
	}
}
