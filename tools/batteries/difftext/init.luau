local myersdiff = require("@rose/batteries/difftext/myersdiff")
local printdiff = require("@rose/batteries/difftext/printdiff")
local types = require("@rose/batteries/difftext/types")

export type diff = types.diff
export type diffoptions = types.diffoptions

local function diff(a: string, b: string, options: diffoptions): diff
	if options and options.byChar then
		return myersdiff.myersdiffbychar(a, b)
	else
		return myersdiff.myersdiffbyline(a, b)
	end
end

return {
	diff = diff,
	prettydiff = printdiff,
}
