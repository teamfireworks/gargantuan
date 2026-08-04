#pragma once

#include "gargantuan.runtime/StackValue.hpp"
#include "gargantuan.runtime/UserdataDefinition.hpp"
#include "gargantuan.runtime/UserdataMethod.hpp"
#include "gargantuan.runtime/UserdataProperty.hpp"

#include <format>
#include <lua.h>
#include <lualib.h>
#include <string_view>
#include <type_traits>
#include <utility>

#define G_USERDATA_IMPL(classType, ...) const classType::Definition classType::DEFINITION{__VA_ARGS__};

#define G_USERDATA_DECL(classType, ...)                                                                                \
	class classType : public Userdata<classType, classType> {                                                          \
	  public:                                                                                                          \
		static const Definition DEFINITION;                                                                            \
		__VA_ARGS__                                                                                                    \
	};                                                                                                                 \
	template <> struct StackValue<classType> {                                                                         \
		typedef Userdata<classType, classType> This;                                                                   \
		static inline std::string_view ReflectedTypedef() {                                                            \
			return StackValue<This>::ReflectedTypedef();                                                               \
		};                                                                                                             \
		static bool Is(lua_State *L, int idx) {                                                                        \
			return StackValue<This>::Is(L, idx);                                                                       \
		};                                                                                                             \
		static classType From(lua_State *L, int idx) {                                                                 \
			return StackValue<This>::From(L, idx);                                                                     \
		};                                                                                                             \
		static int Push(lua_State *L, classType value) {                                                               \
			return StackValue<This>::Push(L, value);                                                                   \
		};                                                                                                             \
	};

#define G_SHARED_USERDATA_DECL(classType, ...)                                                                         \
	class classType : public std::enable_shared_from_this<classType>,                                                  \
					  public Userdata<classType, std::shared_ptr<classType>> {                                         \
	  public:                                                                                                          \
		static const Definition DEFINITION;                                                                            \
		__VA_ARGS__                                                                                                    \
	};                                                                                                                 \
	template <> struct StackValue<std::shared_ptr<classType>> {                                                        \
		typedef Userdata<classType, std::shared_ptr<classType>> This;                                                  \
		static inline std::string_view ReflectedTypedef() {                                                            \
			return StackValue<This>::ReflectedTypedef();                                                               \
		}                                                                                                              \
		static bool Is(lua_State *L, int idx) {                                                                        \
			return StackValue<This>::Is(L, idx);                                                                       \
		}                                                                                                              \
		static std::shared_ptr<classType> From(lua_State *L, int idx) {                                                \
			return StackValue<This>::From(L, idx);                                                                     \
		}                                                                                                              \
		static int Push(lua_State *L, std::shared_ptr<classType> value) {                                              \
			return StackValue<This>::Push(L, value);                                                                   \
		}                                                                                                              \
	};

namespace gargantuan {
	template <typename Class, typename StoredAs = Class> class Userdata {
	  public:
		typedef Class ClassType;
		typedef StoredAs StoredAsType;
		typedef Userdata<ClassType, StoredAsType> Self;
		typedef UserdataDefinition<ClassType> Definition;
		typedef UserdataProperty<ClassType> Property;
		typedef UserdataMethod<ClassType> Method;

		static int DefaultIndex(lua_State *L) {
			ClassType *instance = fromStackValue(L, 1);
			std::string_view key = CheckStackValue<std::string_view>(L, 2);

			if (!instance) {
				return 0;
			}

			const Definition &definition = ClassType::DEFINITION;
			if (auto it = definition.Properties.find(key); it != definition.Properties.end()) {
				const Property &property = it->second;
				if (property.Read) {
					auto value = property.Read(instance);
					return property.PushStack(L, value);
				}
				return 0;
			} else if (auto it = definition.Methods.find(key); it != definition.Methods.end()) {
				PushMethodAsClosure(L, it->first, it->second);
				return 1;
			}

			return 0;
		};

		static int DefaultNewIndex(lua_State *L) {
			ClassType *instance = fromStackValue(L, 1);
			std::string_view key = CheckStackValue<std::string_view>(L, 2);

			if (!instance) {
				return 0;
			}

			const Definition &definition = ClassType::DEFINITION;
			if (auto it = definition.Properties.find(key); it != definition.Properties.end()) {
				const Property &property = it->second;
				if (property.Write) {
					auto newValue = property.CheckStack(L, 3);
					property.Write(instance, newValue);
				} else {
					luaL_error(L, "%s is read-only", key.data());
				}
				return 0;
			}

			luaL_error(L, "Unknown property named %s", std::string(key).c_str());
			return 0;
		};

		static int DefaultNamecall(lua_State *L) {
			ClassType *instance = fromStackValue(L, 1);
			const char *key = lua_namecallatom(L, nullptr);
			if (!key || !instance) {
				luaL_error(L, "Missing instance or method name");
				return 0;
			}

			const Definition &definition = ClassType::DEFINITION;
			if (auto it = definition.Methods.find(key); it != definition.Methods.end()) {
				const Method &method = it->second;
				return method.Call(L, instance);
			}

			luaL_error(L, "Unknown method named %s", key);
			return 0;
		};

		static int DefaultTostring(lua_State *L) {
			const Definition &definition = ClassType::DEFINITION;
			lua_pushlstring(L, definition.Type.data(), definition.Type.size());
			return 1;
		};

		static void CreateUserdataMetatable(lua_State *L) {
			const Definition &definition = ClassType::DEFINITION;
			std::string typeString(definition.Type.data(), definition.Type.size());

			lua_createtable(L, 0, 0);

			lua_pushstring(L, typeString.c_str());
			lua_setfield(L, -2, "__type");

			lua_pushcfunction(L, ClassType::DefaultIndex, std::format("{}.__index", typeString).c_str());
			lua_setfield(L, -2, "__index");

			lua_pushcfunction(L, ClassType::DefaultNewIndex, std::format("{}.__newindex", typeString).c_str());
			lua_setfield(L, -2, "__newindex");

			lua_pushcfunction(L, ClassType::DefaultNamecall, "__namecall");
			lua_setfield(L, -2, "__namecall");

			lua_pushcfunction(L, ClassType::DefaultTostring, std::format("{}.__tostring", typeString).c_str());
			lua_setfield(L, -2, "__tostring");

			for (const auto &[name, method] : definition.Methods) {
				if (name.starts_with("__")) {
					PushMethodAsClosure(L, name, method);
					lua_setfield(L, -2, name.data());
				};
			}

			lua_setreadonly(L, -1, true);
			lua_ref(L, -1);
			lua_setuserdatametatable(L, (int)definition.Tag);
			lua_setuserdatadtor(L, (int)definition.Tag, [](lua_State *, void *userdata) {
				static_cast<StoredAs *>(userdata)->~StoredAs();
			});
		};

	  private:
		template <typename T, typename = std::void_t<>> struct HasGetter : std::false_type {};
		template <typename T> struct HasGetter<T, std::void_t<decltype(std::declval<T>().get())>> : std::true_type {};

		static ClassType *fromStackValue(lua_State *L, int idx) {
			const Definition &definition = ClassType::DEFINITION;
			StoredAsType *instancePointer = static_cast<StoredAsType *>(
				lua_touserdatatagged(L, idx, (int)definition.Tag)
			);
			if (!instancePointer) {
				return nullptr;
			};

			ClassType *instance = nullptr;
			if constexpr (std::is_pointer_v<StoredAsType>) {
				instance = *instancePointer;
			} else if constexpr (HasGetter<StoredAsType>::value) {
				instance = instancePointer->get();
			} else {
				instance = instancePointer;
			};

			return instance;
		}

		static void PushMethodAsClosure(lua_State *L, std::string_view name, const Method &method) {
			const Definition &definition = ClassType::DEFINITION;

			std::string typeString(definition.Type.data(), definition.Type.size());
			std::string methodName(name.data(), name.size());
			std::string debugString = std::format("{}.{}", typeString, methodName.data());

			auto closure = [](lua_State *L) -> int {
				auto *methodPointer = static_cast<Method *>(lua_touserdata(L, lua_upvalueindex(1)));
				auto self = fromStackValue(L, 1);
				return methodPointer && methodPointer->Call ? methodPointer->Call(L, self) : 0;
			};

			lua_pushlightuserdata(L, const_cast<Method *>(&method));
			lua_pushcclosure(L, closure, debugString.c_str(), 1);
		}
	};

	template <typename T> concept IsUserdataDerived = requires { typename T::Self; } &&
													  std::is_base_of_v<typename T::Self, T>;

	template <typename Class, typename StoredAs> struct StackValue<Userdata<Class, StoredAs>> {
		typedef Userdata<Class, StoredAs> This;

		static inline std::string_view ReflectedTypedef() {
			return Class::DEFINITION.Type;
		}

		static bool Is(lua_State *L, int idx) {
			return lua_userdatatag(L, idx) == static_cast<int>(Class::DEFINITION.Tag);
		}

		static StoredAs From(lua_State *L, int idx) {
			StoredAs *userdata = static_cast<StoredAs *>(
				lua_touserdatatagged(L, idx, static_cast<int>(Class::DEFINITION.Tag))
			);
			return userdata ? *userdata : StoredAs{};
		}

		static int Push(lua_State *L, StoredAs value) {
			StoredAs *userdata = static_cast<StoredAs *>(
				lua_newuserdatataggedwithmetatable(L, sizeof(StoredAs), static_cast<int>(Class::DEFINITION.Tag))
			);
			new (userdata) StoredAs(value);
			return 1;
		}
	};
}
