#pragma once

#include "gargantuan/scripting/StackValue.hpp"
#include <functional>
#include <lualib.h>
#include <string>
#include <utility>

namespace gargantuan {
	template <typename Self> struct UserdataMethod {
	  public:
		int (*Call)(lua_State *L, Self *instance);
		std::string Signature{};

		template <auto MethodPointer, typename Class, typename Returns, typename... Arguments>
		static UserdataMethod fromMember(Returns (Class::*)(Arguments...)) {
			return {.Call = [](lua_State *L, Self *instance) -> int {
				auto *derived = static_cast<Class *>(instance);
				return CallFromMember<MethodPointer, Class, Arguments...>(
					L, derived, std::index_sequence_for<Arguments...>{}
				);
			}};
		}

		template <auto MethodPointer, typename Class, typename Returns, typename... Arguments>
		static UserdataMethod fromMember(Returns (Class::*)(Arguments...) const) {
			return {.Call = [](lua_State *L, Self *instance) -> int {
				auto *derived = static_cast<Class *>(instance);
				return CallFromMember<MethodPointer, Class, Arguments...>(
					L, derived, std::index_sequence_for<Arguments...>{}
				);
			}};
		}

		template <auto MethodPointer> static UserdataMethod fromMember() {
			return fromMember<MethodPointer>(MethodPointer);
		}

	  private:
		template <auto MethodPointer, typename Class, typename... Arguments, std::size_t... Indices>
		static int CallFromMember(lua_State *L, Class *instance, std::index_sequence<Indices...>) {
			using Ret = std::invoke_result_t<decltype(MethodPointer), Class *, std::decay_t<Arguments>...>;

			if constexpr (std::is_void_v<Ret>) {
				std::invoke(MethodPointer, instance, StackValue<std::decay_t<Arguments>>::From(L, Indices + 2)...);
				return 0;
			} else {
				auto &&res = std::invoke(
					MethodPointer, instance, StackValue<std::decay_t<Arguments>>::From(L, Indices + 2)...
				);
				StackValue<std::decay_t<Ret>>::Push(L, std::forward<decltype(res)>(res));
				return 1;
			}
		}
	};
}
