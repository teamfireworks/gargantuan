#pragma once

#include <lua.h>
#include <lualib.h>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

namespace gargantuan {
	template <typename T> struct StackValue {
		static inline std::string_view ReflectedTypedef();
		static bool Is(lua_State *L, int idx);
		static T From(lua_State *L, int idx);
		static int Push(lua_State *L, T value);

	  private:
		// i hope NOBODY has to see the original error message

		template <typename Missing> struct GARGANTUAN_STACK_VALUE_IS_UNIMPLEMENTED_FOR {
			static constexpr bool value = false;
		};

		static_assert(GARGANTUAN_STACK_VALUE_IS_UNIMPLEMENTED_FOR<T>::value);
	};

	template <typename T> concept IsStackValue = requires(lua_State *L, int idx, T val) {
		{ StackValue<T>::ReflectedTypedef() } -> std::convertible_to<std::string_view>;
		{ StackValue<T>::Is(L, idx) } -> std::same_as<bool>;
		{ StackValue<T>::From(L, idx) } -> std::same_as<T>;
		{ StackValue<T>::Push(L, val) } -> std::same_as<int>;
	};

	template <typename T> T CheckStackValue(lua_State *L, int idx) {
		if (StackValue<T>::Is(L, idx)) {
			return StackValue<T>::From(L, idx);
		};
		luaL_typeerror(L, idx, StackValue<T>::ReflectedTypedef().data());
	}

#define PRIMITIVE_STACK_VALUE(cppType, reflectedTypedef, isImpl, fromImpl, pushImpl)                                   \
	template <> struct StackValue<cppType> {                                                                           \
		static inline std::string_view ReflectedTypedef() {                                                            \
			return reflectedTypedef;                                                                                   \
		};                                                                                                             \
		static bool Is(lua_State *L, int idx) {                                                                        \
			return isImpl(L, idx);                                                                                     \
		};                                                                                                             \
		static cppType From(lua_State *L, int idx) {                                                                   \
			return fromImpl(L, idx);                                                                                   \
		};                                                                                                             \
		static int Push(lua_State *L, cppType value) {                                                                 \
			pushImpl(L, value);                                                                                        \
			return 1;                                                                                                  \
		};                                                                                                             \
	};

#define STRING_STACK_VALUE(cppType)                                                                                    \
	template <> struct StackValue<cppType> {                                                                           \
		static inline std::string_view ReflectedTypedef() {                                                            \
			return "string";                                                                                           \
		};                                                                                                             \
		static bool Is(lua_State *L, int idx) {                                                                        \
			return lua_isstring(L, idx);                                                                               \
		};                                                                                                             \
		static cppType From(lua_State *L, int idx) {                                                                   \
			size_t len;                                                                                                \
			const char *str = luaL_checklstring(L, idx, &len);                                                         \
			return {str, len};                                                                                         \
		};                                                                                                             \
		static int Push(lua_State *L, cppType value) {                                                                 \
			lua_pushlstring(L, value.data(), value.length());                                                          \
			return 1;                                                                                                  \
		};                                                                                                             \
	};

	PRIMITIVE_STACK_VALUE(float, "number", lua_isnumber, lua_tonumber, lua_pushnumber);
	PRIMITIVE_STACK_VALUE(double, "number", lua_isnumber, lua_tonumber, lua_pushnumber);
	PRIMITIVE_STACK_VALUE(int, "number", lua_isnumber, lua_tonumber, lua_pushnumber);
	PRIMITIVE_STACK_VALUE(bool, "boolean", lua_isboolean, lua_toboolean, lua_pushboolean);
	PRIMITIVE_STACK_VALUE(const char *, "string", lua_isstring, lua_tostring, lua_pushstring);
	STRING_STACK_VALUE(std::string);
	STRING_STACK_VALUE(std::string_view);

	template <typename... Types> struct StackValue<std::tuple<Types...>> {
		static inline std::string_view ReflectedTypedef() {
			// NOTE: tuples are probably not used for typedef generation right?
			return "any";
		};

		static int Push(lua_State *L, const std::tuple<Types...> &tuple) {
			std::apply(
				[L](const auto &...args) { (StackValue<std::decay_t<decltype(args)>>::Push(L, args), ...); }, tuple
			);
			return sizeof...(Types);
		}

		static std::tuple<Types...> From(lua_State *L, int idx) {
			int absIdx = lua_absindex(L, idx);
			return FromImpl(L, absIdx, std::index_sequence_for<Types...>{});
		}

	  private:
		template <std::size_t... Indices>
		static std::tuple<Types...> FromImpl(lua_State *L, int idx, std::index_sequence<Indices...>) {
			return std::make_tuple(StackValue<std::decay_t<Types>>::From(L, idx + static_cast<int>(Indices))...);
		}
	};

	template <typename T>
		requires IsStackValue<T>
	struct StackValue<std::vector<T>> {
		static inline std::string_view ReflectedTypedef() {
			return std::string("{ ") + StackValue<T>::ReflectedTypedef() + " }";
		};

		static bool Is(lua_State *L, int idx) {
			return lua_istable(L, idx);
		};

		static int Push(lua_State *L, const std::vector<T> &value) {
			auto len = value.size();
			lua_createtable(L, len, 0);
			int tableIdx = lua_gettop(L);
			for (size_t i = 0; i < len; ++i) {
				StackValue<T>::Push(L, value[i]);
				lua_rawseti(L, tableIdx, i + 1);
			}
			return 1;
		}

		static std::vector<T> From(lua_State *L, int idx) {
			std::vector<T> vec;
			vec.resize(lua_objlen(L, idx));

			for (int iter = 0; (iter = lua_rawiter(L, idx, iter)) != -1;) {
				if (lua_isnumber(L, idx) && StackValue<T>::Is(L, -1)) vec.push_back(StackValue<T>::From(L, -1));
				lua_pop(L, 2);
			}

			return vec;
		}
	};

	template <typename T>
		requires IsStackValue<T>
	struct StackValue<std::optional<T>> {
		static inline std::string ReflectedTypedef() {
			return std::string(StackValue<T>::ReflectedTypedef()) + "?";
		}

		static bool Is(lua_State *L, int idx) {
			return lua_isnoneornil(L, idx) || StackValue<T>::Is(L, idx);
		};

		static std::optional<T> From(lua_State *L, int idx) {
			return lua_isnoneornil(L, idx) ? std::optional<T>() : StackValue<T>::From(L, idx);
		};

		static int Push(lua_State *L, std::optional<T> value) {
			if (value.has_value()) {
				return StackValue<T>::Push(L, value.value());
			} else {
				lua_pushnil(L);
				return 1;
			}
		};
	};

	template <> struct StackValue<std::monostate> {
		static inline std::string_view ReflectedTypedef() {
			return "()";
		};

		static bool Is(lua_State *L, int idx) {
			return lua_isnone(L, idx);
		};

		static std::monostate From(lua_State *L, int idx) {
			return {};
		};

		static int Push(lua_State *L, std::monostate value) {
			return 0;
		};
	};
}
