-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const classes = require("@gluten/codegen/classes")

const function formatUserdataMacro(userdata: classes.UserdataSchema)
	return `GG_{string.upper(userdata.Name)}_START`, `GG_{string.upper(userdata.Name)}_END`
end

return formatUserdataMacro
