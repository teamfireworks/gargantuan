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

const function formatUserdataContainer(userdata: classes.UserdataSchema)
	const identifier = formatSchemaIdentifier(userdata)
	return if userdata.Prelude.Container == "Shared"
		then `std::shared_ptr<{identifier}>`
		elseif userdata.Prelude.Container == "Unique" then `std::unique_ptr<{identifier}>`
		else identifier
end

return formatUserdataContainer
