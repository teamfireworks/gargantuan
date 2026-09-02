-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const classes = require("@gluten/codegen/classes")
const formatSchemaIdentifier = require("@gluten/codegen/formatSchemaIdentifier")
const formatUserdataContainer = require("./formatUserdataContainer")
const formatUserdataTag = require("./formatUserdataTag")

const function generateLuaValue(userdata: classes.UserdataSchema, includes: { [string]: true })
	includes['"core/generated/gg_UserdataTag.hpp"'] = true
	includes['"core/reflection/gg_LuaValue.hpp"'] = true

	const container = formatUserdataContainer(userdata)
	const identifier = formatSchemaIdentifier(userdata)
	const tag = formatUserdataTag(userdata)

	const body: { string } = {
		`}; \\`,
		`template <> struct gg_LuaValue<{identifier}> \{ \\`,
		`public: \\`,
		`\tstatic std::string ExpectedType() \{ return {string.format("%q", userdata.Name)}; \}; \\`,
		`\tstatic bool Is(lua_State* L, int idx) \{ return lua_userdatatag(L, idx) == {tag}; \}; \\`,

		`\tstatic {identifier} From(lua_State* L, int idx) \{ \\`,
		`\t\t{container} *userdata = static_cast<{container} *>(lua_touserdatatagged(L, idx, {tag})); \\`,
		`\t\treturn userdata ? *userdata : {container}\{}; \\`,
		`\t}; \\`,

		`\tstatic int Push(lua_State* L, {identifier} value) \{ \\`,
		`\t\t{container} *userdata = static_cast<{container} *>(lua_newuserdatataggedwithmetatable(L, sizeof({container}), {tag})); \\`,
		`\t\tnew (userdata) {container}(value); \\`,
		`\t\treturn 1; \\`,
		`\t};`,
	}

	return body
end

-- StoredAs *userdata = static_cast<StoredAs *>(
-- 				lua_touserdatatagged(L, idx, static_cast<int>(Class::DEFINITION.Tag))
-- 			);
-- 			return userdata ? *userdata : StoredAs{};
-- StoredAs *userdata = static_cast<StoredAs *>(
-- 				lua_newuserdatataggedwithmetatable(L, sizeof(StoredAs), static_cast<int>(Class::DEFINITION.Tag))
-- 			);
-- 			new (userdata) StoredAs(value);
-- 			return 1;
return generateLuaValue
