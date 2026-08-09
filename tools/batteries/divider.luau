const richterm = require("@rose/batteries/richterm")

const LINE = "─"
const DEFAULT_LENGTH = 60

local roseRed = richterm.combine(richterm.brightRed, richterm.dim)

local function divider(label: string?, length: number?): string
	length = length or DEFAULT_LENGTH

	if label then
		local labelLength = string.len(label)
		local lineLength = math.floor((length - labelLength - 2) * 0.5)
		local endPadding = if labelLength % 2 == 1 then 1 else 0
		return roseRed(`{string.rep(LINE, lineLength)} {label} {string.rep(LINE, lineLength + endPadding)}`)
	end

	return roseRed(string.rep(LINE, length))
end

return divider
