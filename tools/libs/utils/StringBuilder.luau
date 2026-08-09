const StringBuilder = {}
StringBuilder.__index = StringBuilder

-- NOTE: Indents are increments of 2 for `public:` accessor syntax
export type StringBuilder = setmetatable<{
	inner: string,
	halfTabs: number,
}, typeof(StringBuilder)>

function StringBuilder.new(): StringBuilder
	return setmetatable({ inner = "", halfTabs = 0 }, StringBuilder)
end

function StringBuilder.formatIndent(self: StringBuilder): string
	return string.rep("\t", self.halfTabs // 2) .. string.rep("  ", self.halfTabs % 2)
end

function StringBuilder.tab(self: StringBuilder): StringBuilder
	self.halfTabs += 2
	return self
end

function StringBuilder.tabHalf(self: StringBuilder): StringBuilder
	self.halfTabs += 1
	return self
end

function StringBuilder.untab(self: StringBuilder): StringBuilder
	self.halfTabs = math.max(self.halfTabs - 2, 0)
	return self
end

function StringBuilder.untabHalf(self: StringBuilder): StringBuilder
	self.halfTabs = math.max(self.halfTabs - 1, 0)
	return self
end

function StringBuilder.push(self: StringBuilder, str: string): StringBuilder
	self.inner ..= str
	return self
end

function StringBuilder.pushIndented(self: StringBuilder, str: string): StringBuilder
	const indent = self:formatIndent()
	self.inner ..= indent .. string.gsub(str, "\n", "\n" .. indent)
	return self
end

function StringBuilder.line(self: StringBuilder, str: string): StringBuilder
	self.inner ..= self:formatIndent() .. str .. "\n"
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
