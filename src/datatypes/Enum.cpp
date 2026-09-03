#include "gargantuan/datatypes/Enum.hpp"
#include "gargantuan/scripting/StackValue.hpp"
#include "gargantuan/scripting/Userdata.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

#include <cstring>
#include <lua.h>
#include <lualib.h>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

namespace gargantuan {
	G_USERDATA_IMPL(
		EnumItem,
		.Tag = UserdataTag::EnumItem,
		.Type = "EnumItem",
		.Properties =
			{
				{"Name", Property::fromMember<&EnumItem::Name>(true, false)},
				{"Value", Property::fromMember<&EnumItem::Value>(true, false)},
				{"EnumType", Property::fromMember<&EnumItem::EnumType>(true, false)},
			},
		.Methods = {
			{"__tostring", Method{&EnumItem::LTostring}},
			{"__eq", Method{&EnumItem::LEq}},
		}
	);

	int EnumItem::LTostring(lua_State *L, EnumItem *self) {
		std::ostringstream ss;
		ss << "Enum." << self->EnumType->Name << "." << self->Name;
		std::string str = ss.str();
		lua_pushlstring(L, str.c_str(), str.size());
		return 1;
	};

	int EnumItem::LEq(lua_State *L, EnumItem *self) {
		if (StackValue<EnumItem>::Is(L, 2)) {
			EnumItem other = StackValue<EnumItem>::From(L, 2);
			lua_pushboolean(
				L, self->Value == other.Value && self->Name == other.Name && self->EnumType == other.EnumType
			);
		} else {
			lua_pushboolean(L, false);
		}
		return 1;
	}

	G_USERDATA_IMPL(
		Enum,
		.Tag = UserdataTag::Enum,
		.Type = "Enum",
		.Methods = {
			{"GetEnumItems", Method::fromMember<&Enum::GetEnumItems>()},
			{"FromName", Method::fromMember<&Enum::FromName>()},
			{"FromValue", Method::fromMember<&Enum::FromValue>()},
			{"__index", Method{&Enum::LIndex}},
			{"__tostring", Method{&Enum::LTostring}},
		}
	);

	std::vector<EnumItem> &Enum::GetEnumItems() {
		return Items;
	};

	std::optional<EnumItem> Enum::FromName(std::string_view name) {
		for (auto &item : Items) {
			if (item.Name == name) {
				return item;
			}
		}
		return {};
	};

	std::optional<EnumItem> Enum::FromValue(int value) {
		for (auto &item : Items) {
			if (item.Value == value) {
				return item;
			}
		}
		return {};
	};

	int Enum::LIndex(lua_State *L, Enum *self) {
		auto key = luaL_checkstring(L, 2);
		if (auto item = self->FromName(key)) {
			StackValue<EnumItem>::Push(L, item.value());
			return 1;
		} else {
			luaL_errorL(L, "%s is not a valid member of \"Enum.%s\"", key, self->Name.data());
			return 0;
		}
	};

	int Enum::LTostring(lua_State *L, Enum *self) {
		StackValue<std::string_view>::Push(L, self->Name);
		return 1;
	};
}
