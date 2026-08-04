#pragma once

#include "gargantuan.runtime/Userdata.hpp"

#include <SDL3/SDL_log.h>
#include <lua.h>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <string_view>
#include <type_traits>
#include <vector>

namespace gargantuan {
	struct Enum;

	G_USERDATA_DECL(
		EnumItem, std::string_view Name = "Invalid"; int Value = 0; std::shared_ptr<Enum> EnumType = nullptr;

		EnumItem() = default;
		EnumItem(std::string_view name, int value, std::shared_ptr<Enum> enumType = nullptr) : Name(name),
		Value(value),
		EnumType(std::move(enumType)) {}

		static int LTostring(lua_State *L, EnumItem *self);
	);

	namespace Enums {
		std::unordered_map<std::string_view, std::shared_ptr<Enum>> &GetEnums();
	}

	G_SHARED_USERDATA_DECL(
		Enum, typedef std::shared_ptr<Enum> Pointer; typedef Userdata<Enum, Pointer> Self;

		std::string_view Name;
		std::vector<EnumItem> Items;

		template <typename E> requires std::is_enum_v<E> static typename Enum::Pointer fromType() {
			static const Enum::Pointer self = []() {
				Enum::Pointer result = std::make_shared<Enum>();
				result->Name = magic_enum::enum_type_name<E>();

				constexpr auto entries = magic_enum::enum_entries<E>();
				result->Items.reserve(entries.size());

				for (const auto &[value, name] : entries) {
					result->Items.emplace_back(name, static_cast<int>(value), result);
				}

				return result;
			}();
			return self;
		}

		std::vector<EnumItem> &
		GetEnumItems();
		std::optional<EnumItem> FromName(std::string_view name);
		std::optional<EnumItem> FromValue(int value);
		static int LIndex(lua_State *L, Enum *self);
		static int LTostring(lua_State *L, Enum *self);
	);
}
