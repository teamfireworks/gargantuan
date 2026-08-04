local Workspace = game:GetService("Workspace")

local baseplate = Instance.new("Part")
baseplate.Color = Color3.fromHSV(0, 0, 0.5)
baseplate.Position = Vector3.new(0, -5, 0)
baseplate.Size = Vector3.new(200, 5, 200)
baseplate.Parent = Workspace

local part = Instance.new("Part")
part.Color = Color3.fromHSV(0, 0, 1)
part.Position = Vector3.new(0, 0, 0)
part.Size = Vector3.new(5, 5, 5)
part.Parent = Workspace
