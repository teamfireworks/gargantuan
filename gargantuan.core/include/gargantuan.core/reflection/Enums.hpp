#pragma once

#include "gargantuan/datatypes/Enum.hpp"

#include <format>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

// Has gargantuan gone too far?
#define G_ENUM(name, ...)                                                                                              \
	namespace Enums {                                                                                                  \
		enum class name : int { __VA_ARGS__ };                                                                         \
		namespace __IGNORE__ {                                                                                         \
			inline static const bool name = []() {                                                                     \
				Enums::Register(Enum::fromType<Enums::name>());                                                        \
				return true;                                                                                           \
			}();                                                                                                       \
		}                                                                                                              \
	}

namespace gargantuan::Enums {
	std::unordered_map<std::string_view, Enum::Pointer> &GetEnums();
	void Register(Enum::Pointer self);
}

// NOTE: this assumes you registered an EnumItem for this Enum
template <typename E>
	requires std::is_enum_v<E>
struct gargantuan::StackValue<E> {
  public:
	static inline std::string ReflectedTypedef() {
		return std::format("Enum.{}", ENUM_NAME);
	};

	static bool Is(lua_State *L, int idx) {
		return StackValue<EnumItem>::Is(L, idx);
	};

	static E From(lua_State *L, int idx) {
		return (E)StackValue<EnumItem>::From(L, idx).Value;
	};

	static int Push(lua_State *L, E value) {
		Enum::Pointer self = GetEnum();
		for (auto &item : self->Items) {
			if (item.Value == (int)value) {
				return StackValue<EnumItem>::Push(L, item);
			}
		}
		throw std::runtime_error(std::format("Invalid value for Enum.{}", ENUM_NAME));
	};

  private:
	static constexpr std::string_view ENUM_NAME = magic_enum::enum_type_name<E>();

	static Enum::Pointer GetEnum() {
		auto &enums = gargantuan::Enums::GetEnums();
		if (auto it = enums.find(ENUM_NAME); it != enums.end()) {
			return it->second;
		} else {
			throw std::runtime_error(
				std::format(
					"Cannot use {} as a stack value because it is not registered as an Enum userdata", ENUM_NAME.data()
				)
			);
		}
	}
};
