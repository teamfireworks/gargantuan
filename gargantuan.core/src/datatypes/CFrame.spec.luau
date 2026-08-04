local Lest = require("@lest")
local describe, it, expect = Lest.describe, Lest.it, Lest.expect

describe("CFrame", function()
	it("constructs with no arguments", function()
		local self = CFrame.new()
		expect(self.X).toBe(0)
		expect(self.Y).toBe(0)
		expect(self.Z).toBe(0)
	end)

	it("constructs with XYZ arguments", function()
		local self = CFrame.new(1, 2, 3)
		expect(self.X).toBe(1)
		expect(self.Y).toBe(2)
		expect(self.Z).toBe(3)
	end)

	it("returns correct RightVector", function()
		local self = CFrame.new()
		expect(self.RightVector.X).toBe(1)
		expect(self.RightVector.Y).toBe(0)
		expect(self.RightVector.Z).toBe(0)
	end)

	it("returns correct UpVector", function()
		local self = CFrame.new()
		expect(self.UpVector.X).toBe(0)
		expect(self.UpVector.Y).toBe(1)
		expect(self.UpVector.Z).toBe(0)
	end)

	it("returns correct LookVector", function()
		local self = CFrame.new()
		expect(self.LookVector.X).toBe(0)
		expect(self.LookVector.Y).toBe(0)
		expect(self.LookVector.Z).toBe(-1)
	end)

	it("returns identity when inversing identity", function()
		local self = CFrame.new():Inverse()
		expect(self.X).toBe(0)
		expect(self.Y).toBe(0)
		expect(self.Z).toBe(0)
	end)

	it("applies translation with multiplication", function()
		local lhs = CFrame.new(1, 0, 0)
		local rhs = CFrame.new(0, 2, 0)
		local res = lhs * rhs
		expect(res.X).toBe(1)
		expect(res.Y).toBe(2)
		expect(res.Z).toBe(0)
	end)

	it("lerps startpoint", function()
		local start = CFrame.new(0, 0, 0)
		local goal = CFrame.new(10, 20, 30)

		local res = start:Lerp(goal, 0)
		expect(res.X).toBe(0)
		expect(res.Y).toBe(0)
		expect(res.Z).toBe(0)
	end)

	it("lerps endpoint", function()
		local start = CFrame.new(0, 0, 0)
		local goal = CFrame.new(10, 20, 30)

		local res = start:Lerp(goal, 1)
		expect(res.X).toBe(10)
		expect(res.Y).toBe(20)
		expect(res.Z).toBe(30)
	end)

	it("lerps midpoint", function()
		local start = CFrame.new(0, 0, 0)
		local goal = CFrame.new(10, 20, 30)

		local res = start:Lerp(goal, 0.5)
		expect(res.X).toBe(5)
		expect(res.Y).toBe(10)
		expect(res.Z).toBe(15)
	end)

	-- TODO: spec out the rest of https://github.com/Qquaded/Kinemium-Engine/blob/main/src/test/cframe_test.luau
end)
