#pragma once

#include "gargantuan.runtime/StackValue.hpp"

#include <any>
#include <functional>
#include <lualib.h>
#include <string_view>

namespace gargantuan {
	template <typename Self> struct UserdataProperty {
		std::string_view (*GetReadTypedef)();
		int (*PushStack)(lua_State *L, std::any value) = nullptr;
		std::function<std::any(Self *)> Read = nullptr;

		std::string_view (*GetWriteTypedef)();
		std::any (*CheckStack)(lua_State *L, int idx) = nullptr;
		std::function<void(Self *, std::any)> Write = nullptr;

		bool Serializable = false;

		template <typename MemberType> struct MemberTraits;
		template <typename C, typename T> struct MemberTraits<T C::*> {
			using Target = C;
			using Value = T;
		};

		template <auto MemberPointer>
		static UserdataProperty fromMember(bool enableRead = false, bool enableWrite = false) {
			using MemberClass = typename MemberTraits<decltype(MemberPointer)>::Target;
			using Value = typename MemberTraits<decltype(MemberPointer)>::Value;

			UserdataProperty self{nullptr, nullptr};

			if (enableRead) {
				self.GetReadTypedef = []() -> std::string_view {
					static const auto typeName = StackValue<Value>::ReflectedTypedef();
					return typeName;
				};
				self.PushStack = [](lua_State *L, std::any value) -> int {
					return StackValue<Value>::Push(L, std::any_cast<Value>(value));
				};
				self.Read = [](Self *instance) -> std::any {
					return static_cast<MemberClass *>(instance)->*MemberPointer;
				};
			}

			if (enableWrite) {
				self.GetWriteTypedef = []() -> std::string_view {
					static const auto typeName = StackValue<Value>::ReflectedTypedef();
					return typeName;
				};
				self.CheckStack = [](lua_State *L, int idx) -> std::any { return CheckStackValue<Value>(L, idx); };
				self.Write = [](Self *instance, std::any value) {
					static_cast<MemberClass *>(instance)->*MemberPointer = std::any_cast<Value>(value);
				};
			}

			return self;
		}

		template <auto MemberPointer> static UserdataProperty fromReadonlyMember() {
			return fromMember<MemberPointer>(true, false);
		}

		template <auto MemberPointer> static UserdataProperty fromWriteonlyMember() {
			return fromMember<MemberPointer>(false, true);
		}

		template <auto MemberPointer> static UserdataProperty fromReadWriteMember() {
			return fromMember<MemberPointer>(true, true);
		}

		template <typename Reader> static UserdataProperty fromRead(Reader &&read) {
			using ReadType = std::invoke_result_t<Reader, Self *>;
			UserdataProperty self;

			self.GetReadTypedef = []() -> std::string_view {
				static const auto typeName = StackValue<ReadType>::ReflectedTypedef();
				return typeName;
			};
			self.PushStack = [](lua_State *L, std::any value) -> int {
				return StackValue<ReadType>::Push(L, std::any_cast<ReadType>(value));
			};
			self.Read = [reader = std::forward<Reader>(read)](Self *instance) -> std::any { return reader(instance); };

			return self;
		}

		template <typename WriteType, typename Reader, typename Writer>
		static UserdataProperty fromReadWrite(Reader &&read, Writer &&write) {
			UserdataProperty self;

			using ReadType = std::invoke_result_t<Reader, Self *>;
			self.GetReadTypedef = []() -> std::string_view {
				static const auto typeName = StackValue<ReadType>::ReflectedTypedef();
				return typeName;
			};
			self.PushStack = [](lua_State *L, std::any value) -> int {
				return StackValue<ReadType>::Push(L, std::any_cast<ReadType>(value));
			};
			self.Read = [reader = std::forward<Reader>(read)](Self *instance) -> std::any { return reader(instance); };

			self.GetWriteTypedef = []() -> std::string_view {
				static const auto typeName = StackValue<WriteType>::ReflectedTypedef();
				return typeName;
			};
			self.CheckStack = [](lua_State *L, int idx) -> std::any { return CheckStackValue<WriteType>(L, idx); };
			self.Write = [writer = std::forward<Writer>(write)](Self *instance, std::any value) {
				writer(instance, std::any_cast<WriteType>(value));
			};

			return self;
		}

		UserdataProperty<Self> &SetSerializable(bool enabled = true) {
			Serializable = enabled;
			return *this;
		}
	};
}
