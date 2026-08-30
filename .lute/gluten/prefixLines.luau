-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

local tableext = require("@std/tableext")

const function prefixLines(prefix: string, lines: { string })
	return tableext.map(lines, function(line)
		return prefix .. line
	end)
end

return prefixLines
