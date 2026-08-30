-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

local process = require("@std/process")

const function countCpus(): number
	-- Windows
	const envNumProcessors = process.env["NUMBER_OF_PROCESSORS"]
	if envNumProcessors then
		const numProcessors = tonumber(envNumProcessors)
		if numProcessors and numProcessors > 0 then
			return numProcessors
		end
	end

	-- Linux
	const nprocResult = process.run({ "nproc", "--all" })
	if nprocResult.ok then
		const numProcessors = tonumber(nprocResult.stdout)
		if numProcessors and numProcessors > 0 then
			return numProcessors
		end
	end

	-- MacOS
	const sysResult = process.run({ "sysctl", "-n", "hw.ncpu" })
	if sysResult.ok then
		const numProcessors = tonumber(sysResult.stdout)
		if numProcessors and numProcessors > 0 then
			return numProcessors
		end
	end

	return 1
end

return countCpus
