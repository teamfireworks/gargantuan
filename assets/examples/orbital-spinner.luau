local Workspace = game:GetService("Workspace")

const SPINNER_CENTER = Vector3.new(0, 128, 0)

local baseplate = Instance.new("Part")
baseplate.Anchored = true
baseplate.Color = Color3.fromHSV(0, 0, 0.5)
baseplate.Position = Vector3.new(0, -8, 0)
baseplate.Size = Vector3.new(1000, 16, 1000)
baseplate.Parent = Workspace

local north = Instance.new("Part")
north.Anchored = false
north.Color = Color3.fromHSV(0, 1, 0.75)
north.Position = SPINNER_CENTER + Vector3.new(0, 0, 4)
north.Size = Vector3.new(4, 1, 4)
north.Parent = Workspace

local south = Instance.new("Part")
south.Anchored = false
south.Color = Color3.fromHSV(0.5, 1, 0.75)
south.Position = SPINNER_CENTER + Vector3.new(0, 0, -4)
south.Size = Vector3.new(4, 1, 4)
south.Parent = Workspace

local weld = Instance.new("WeldConstraint")
weld.Part0 = north
weld.Part1 = south
weld.Parent = Workspace
