const ctypes = {}
ctypes.__index = ctypes

export type CType = setmetatable<{
	cType: string,
	cIncludes: { string }?,
	luauType: string,
}, typeof(ctypes)>

return table.freeze(ctypes)
