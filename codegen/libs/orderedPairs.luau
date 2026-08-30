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
