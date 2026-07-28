#pragma once

#include "gargantuan/scripting/StackValue.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

#include <any>
#include <functional>
#include <lua.h>
#include <lualib.h>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace gargantuan {
	template <typename Class, typename StoredAs = Class> class Userdata {
	  public:
		typedef Userdata<Class, StoredAs> This;

		struct Property {
			using Self = Property;
			int (*PushStack)(lua_State *L, std::any value) = nullptr;
			std::any (*Read)(Class *instance) = nullptr;

			std::any (*CheckStack)(lua_State *L, int idx) = nullptr;
			void (*Write)(Class *instance, std::any value) = nullptr;

			// Only relevant with instances, but I'm too lazy to make a
			// ClassProperty and then do some bullshit for the constructors
			bool Serializable = false;

			template <typename MemberType> struct MemberTraits;
			template <typename C, typename T> struct MemberTraits<T C::*> {
				using Target = C;
				using Value = T;
			};

			template <auto MemberPointer>
			static Property fromSimple(bool enableRead = false, bool enableWrite = false) {
				using MemberClass = typename MemberTraits<decltype(MemberPointer)>::Target;
				using Value = typename MemberTraits<decltype(MemberPointer)>::Value;

				Property self;

				if (enableRead) {
					self.PushStack = [](lua_State *L, std::any value) -> int {
						return StackValue<Value>::Push(L, std::any_cast<Value>(value));
					};
					self.Read = [](Class *instance) -> std::any {
						return static_cast<MemberClass *>(instance)->*MemberPointer;
					};
				}

				if (enableWrite) {
					self.CheckStack = [](lua_State *L, int idx) -> std::any { return CheckStackValue<Value>(L, idx); };
					self.Write = [](Class *instance, std::any value) {
						static_cast<MemberClass *>(instance)->*MemberPointer = std::any_cast<Value>(value);
					};
				}

				return self;
			}

			template <typename Reader> static Property fromRead(Reader &&read) {
				using ReadType = std::invoke_result_t<Reader, Class *>;

				Property self;

				self.PushStack = [](lua_State *L, std::any value) -> int {
					return StackValue<ReadType>::Push(L, std::any_cast<ReadType>(value));
				};
				static auto storedRead = std::forward<Reader>(read);
				self.Read = [](Class *instance) -> std::any { return storedRead(instance); };

				return self;
			}

			template <typename WriteType, typename Reader, typename Writer>
			static Property fromReadWrite(Reader &&read, Writer &&write) {
				using ReadType = std::invoke_result_t<Reader, Class *>;
				Property self;

				self.PushStack = [](lua_State *L, std::any value) -> int {
					return StackValue<ReadType>::Push(L, std::any_cast<ReadType>(value));
				};
				static auto storedRead = std::forward<Reader>(read);
				self.Read = [](Class *instance) -> std::any { return storedRead(instance); };

				self.CheckStack = [](lua_State *L, int idx) -> std::any { return CheckStackValue<WriteType>(L, idx); };
				static auto storedWrite = std::forward<Writer>(write);
				self.Write = [](Class *instance, std::any value) {
					storedWrite(instance, std::any_cast<WriteType>(value));
				};

				return self;
			}

			Property &SetSerializable(bool serializable = true) {
				Serializable = serializable;
				return *this;
			}
		};

		struct Method {
		  public:
			int (*Call)(lua_State *L, Class *instance);

			template <auto MethodPointer, typename TargetClass, typename Returns, typename... Arguments>
			static Method Wrap(Returns (TargetClass::*)(Arguments...)) {
				return {[](lua_State *L, Class *instance) -> int {
					auto *derived = static_cast<TargetClass *>(instance);
					return WrappedCall<MethodPointer, TargetClass, Arguments...>(
						L, derived, std::index_sequence_for<Arguments...>{}
					);
				}};
			}

			template <auto MethodPointer, typename TargetClass, typename Returns, typename... Arguments>
			static Method Wrap(Returns (TargetClass::*)(Arguments...) const) {
				return {[](lua_State *L, Class *instance) -> int {
					auto *derived = static_cast<TargetClass *>(instance);
					return WrappedCall<MethodPointer, TargetClass, Arguments...>(
						L, derived, std::index_sequence_for<Arguments...>{}
					);
				}};
			}

			template <auto MethodPointer> static Method Wrap() {
				return Wrap<MethodPointer>(MethodPointer);
			}

		  private:
			template <auto MethodPointer, typename TargetClass, typename... Arguments, std::size_t... Indices>
			static int WrappedCall(lua_State *L, TargetClass *instance, std::index_sequence<Indices...>) {
				using Ret = std::invoke_result_t<decltype(MethodPointer), TargetClass *, std::decay_t<Arguments>...>;

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

		typedef std::unordered_map<std::string_view, Property> UserdataProperties;
		typedef std::unordered_map<std::string_view, Method> UserdataMethods;

		static UserdataTag GetUserdataTag() {
			return Class::GetUserdataTag();
		};

		static std::string_view GetUserdataType() {
			return Class::GetUserdataType();
		};

		static const UserdataProperties &GetUserdataProperties() {
			return Class::GetUserdataProperties();
		};

		static const UserdataMethods &GetUserdataMethods() {
			return Class::GetUserdataMethods();
		};

		static int UserdataIndex(lua_State *L) {
			Class *instance = fromStackValue(L, 1);
			std::string_view key = CheckStackValue<std::string_view>(L, 2);

			if (!instance) {
				return 0;
			}

			const UserdataProperties &properties = Class::GetUserdataProperties();
			if (auto it = properties.find(key); it != properties.end()) {
				const Property &property = it->second;
				if (property.Read) {
					auto value = property.Read(instance);
					return property.PushStack(L, value);
				}
				return 0;
			}

			return 0;
		};

		static int UserdataNewIndex(lua_State *L) {
			Class *instance = fromStackValue(L, 1);
			std::string_view key = CheckStackValue<std::string_view>(L, 2);

			if (!instance) {
				return 0;
			}

			const UserdataProperties &properties = Class::GetUserdataProperties();
			if (auto it = properties.find(key); it != properties.end()) {
				const Property &property = it->second;
				if (property.Write) {
					auto value = property.CheckStack(L, 3);
					property.Write(instance, value);
				} else {
					luaL_error(L, "%s is read-only", key.data());
				}
				return 0;
			}

			return 0;
		};

		static int UserdataNamecall(lua_State *L) {
			Class *instance = fromStackValue(L, 1);
			const char *key = lua_namecallatom(L, nullptr);
			if (!key || !instance) {
				luaL_error(L, "Missing instance or method name");
				return 0;
			}

			const UserdataMethods &methods = Class::GetUserdataMethods();
			if (auto it = methods.find(key); it != methods.end()) {
				const Method &method = it->second;
				return method.Call(L, instance);
			}

			luaL_error(L, "Unknown method named %s", key);
			return 0;
		};

		static int UserdataTostring(lua_State *L) {
			lua_pushstring(L, Class::GetUserdataType().data());
			return 1;
		};

		static void CreateUserdataMetatable(lua_State *L) {
			lua_createtable(L, 0, 0);

			lua_pushstring(L, Class::GetUserdataType().data());
			lua_setfield(L, -2, "__type");

			lua_pushcfunction(L, Class::UserdataIndex, "__index");
			lua_setfield(L, -2, "__index");

			lua_pushcfunction(L, Class::UserdataNewIndex, "__newindex");
			lua_setfield(L, -2, "__newindex");

			lua_pushcfunction(L, Class::UserdataNamecall, "__namecall");
			lua_setfield(L, -2, "__namecall");

			lua_pushcfunction(L, Class::UserdataTostring, "__tostring");
			lua_setfield(L, -2, "__tostring");

			for (const auto &[name, method] : Class::GetUserdataMethods()) {
				if (!name.starts_with("__")) {
					continue;
				}

				lua_pushlightuserdata(L, const_cast<Method *>(&method));
				lua_pushcclosure(
					L,
					[](lua_State *L) -> int {
						auto *methodPtr = static_cast<Method *>(lua_touserdata(L, lua_upvalueindex(1)));
						auto self = fromStackValue(L, 1);
						if (!methodPtr || !methodPtr->Call) {
							return 0;
						}
						return methodPtr->Call(L, self);
					},
					name.data(),
					1
				);

				lua_setfield(L, -2, name.data());
			}

			lua_pushvalue(L, -1);
			lua_setreadonly(L, -1, true);
			lua_setuserdatametatable(L, (int)Class::GetUserdataTag());
		};

	  private:
		template <typename T, typename = std::void_t<>> struct HasGetter : std::false_type {};
		template <typename T> struct HasGetter<T, std::void_t<decltype(std::declval<T>().get())>> : std::true_type {};

		static Class *fromStackValue(lua_State *L, int idx) {
			StoredAs *instancePointer =
				static_cast<StoredAs *>(lua_touserdatatagged(L, idx, (int)Class::GetUserdataTag()));
			if (!instancePointer) {
				return nullptr;
			};

			Class *instance = nullptr;
			if constexpr (std::is_pointer_v<StoredAs>) {
				instance = *instancePointer;
			} else if constexpr (HasGetter<StoredAs>::value) {
				instance = instancePointer->get();
			} else {
				instance = instancePointer;
			};

			return instance;
		}
	};

	template <typename Class, typename StoredAs> struct StackValue<Userdata<Class, StoredAs>> {
		typedef Userdata<Class, StoredAs> This;

		static inline std::string_view ReflectedTypedef() {
			return This::GetUserdataType();
		};

		static bool Is(lua_State *L, int idx) {
			return lua_userdatatag(L, idx) == (int)This::GetUserdataTag();
		};

		static StoredAs From(lua_State *L, int idx) {
			StoredAs *userdata = static_cast<StoredAs *>(lua_touserdatatagged(L, idx, (int)This::GetUserdataTag()));
			return *userdata;
		};

		static int Push(lua_State *L, StoredAs value) {
			StoredAs *userdata = static_cast<StoredAs *>(
				lua_newuserdatataggedwithmetatable(L, sizeof(StoredAs), (int)This::GetUserdataTag())
			);
			new (userdata) StoredAs(value);
			return 1;
		};
	};

#define G_MEMBER_PROPERTY(classType, propertyName, enableRead, enableWrite)                                            \
	{#propertyName, Property::fromMember<&classType::propertyName>(enableRead, enableWrite)}

#define G_UD_STACKVALUE_WITH_STORED(classType, storedType)                                                             \
	template <> struct StackValue<storedType> {                                                                        \
		typedef Userdata<classType, storedType> This;                                                                  \
		static inline std::string_view ReflectedTypedef() {                                                            \
			return StackValue<This>::ReflectedTypedef();                                                               \
		};                                                                                                             \
		static bool Is(lua_State *L, int idx) {                                                                        \
			return StackValue<This>::Is(L, idx);                                                                       \
		};                                                                                                             \
		static storedType From(lua_State *L, int idx) {                                                                \
			return StackValue<This>::From(L, idx);                                                                     \
		};                                                                                                             \
		static int Push(lua_State *L, storedType value) {                                                              \
			return StackValue<This>::Push(L, value);                                                                   \
		};                                                                                                             \
	};

#define G_UD_STACKVALUE(classType) G_UD_STACKVALUE_WITH_STORED(classType, classType)

#define G_UD_METHOD(classType, methodName) {#methodName, Method::Wrap<&classType::methodName>()}

#define G_UD_DECL_PRELUDE(self)                                                                                        \
	static std::string_view GetUserdataType();                                                                         \
	static UserdataTag GetUserdataTag();                                                                               \
	static const self::UserdataProperties &GetUserdataProperties();                                                    \
	static const self::UserdataMethods &GetUserdataMethods();

#define G_UD_IMPL_PRELUDE(self)                                                                                        \
	std::string_view self::GetUserdataType() {                                                                         \
		return #self;                                                                                                  \
	};                                                                                                                 \
	UserdataTag self::GetUserdataTag() {                                                                               \
		return UserdataTag::self;                                                                                      \
	};

#define G_UD_IMPL_PROPS(self, ...)                                                                                     \
	const self::UserdataProperties &self::GetUserdataProperties() {                                                    \
		static const UserdataProperties PROPERTIES = {__VA_ARGS__};                                                    \
		return PROPERTIES;                                                                                             \
	};

#define G_UD_IMPL_METHODS(self, ...)                                                                                   \
	const self::UserdataMethods &self::GetUserdataMethods() {                                                          \
		static const UserdataMethods METHODS = {__VA_ARGS__};                                                          \
		return METHODS;                                                                                                \
	};

} // namespace gargantuan
