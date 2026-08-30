-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const function orderedPairs<K, V>(tbl: { [K]: V }): () -> (K, V)
	const keys: { K } = {}
	local numKeys = 0
	for key in tbl do
		table.insert(keys, key)
		numKeys += 1
	end
	table.sort(keys)

	local currentIndex = 0
	return function()
		currentIndex += 1
		if currentIndex > numKeys then
			return nil, nil
		end

		const key = keys[currentIndex]
		return key, tbl[key]
	end
end

return orderedPairs
