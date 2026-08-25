const function append<T>(src: { T }, dest: { T })
	table.move(src, 1, #src, #dest + 1, dest)
end

return append
