const StringBuilder = {}
StringBuilder.__index = StringBuilder

export type StringBuilder = setmetatable<{ inner: string }, typeof(StringBuilder)>

function StringBuilder.new(): StringBuilder
	return setmetatable({ inner = "" }, StringBuilder)
end

function StringBuilder.push(self: StringBuilder, str: string): StringBuilder
	self.inner ..= str
	return self
end

function StringBuilder.line(self: StringBuilder, str: string): StringBuilder
	self.inner ..= str .. "\n"
	return self
end

function StringBuilder.newline(self: StringBuilder): StringBuilder
	self.inner ..= "\n"
	return self
end

function StringBuilder.build(self: StringBuilder): string
	return self.inner
end

return table.freeze(StringBuilder)
