local Workspace = game:GetService("Workspace")
local UserInputService = game:GetService("UserInputService")

local baseplate = Instance.new("Part")
baseplate.Anchored = true
baseplate.Color = Color3.fromHSV(0, 0, 0.5)
baseplate.Position = Vector3.new(0, -8, 0)
baseplate.Size = Vector3.new(1000, 16, 1000)
baseplate.Parent = Workspace

local part0 = Instance.new("Part")
part0.Anchored = false
part0.Color = Color3.fromRGB(150,0,0) --red
part0.Position = Vector3.new(-5, 0, 0)
part0.Size = Vector3.new(2, 2, 2)
part0.Parent = Workspace

local part1 = Instance.new("Part")
part1.Anchored = false
part1.Color = Color3.fromRGB(0,0,150) --blue
part1.Position = Vector3.new(2, 1, 0)
part1.Size = Vector3.new(2, 2, 2)
part1.Parent = Workspace

local wc = Instance.new("WeldConstraint")
wc.Part0 = part0
wc.Part1 = part1
wc.Enabled = true
wc.Parent = Workspace

UserInputService.JumpRequest:Connect(function()
	part0:ApplyImpulse(Vector3.new(0,5,0))
end)
