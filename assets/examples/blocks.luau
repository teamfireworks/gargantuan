local Workspace = game:GetService("Workspace")

const INTERVAL_SECONDS = 0.1

local baseplate = Instance.new("Part")
baseplate.Anchored = true
baseplate.Color = Color3.fromHSV(0, 0, 0.5)
baseplate.Position = Vector3.new(0, -8, 0)
baseplate.Size = Vector3.new(1000, 16, 1000)
baseplate.Parent = Workspace

Workspace.CurrentCamera.CFrame = CFrame.new(Vector3.new(0, 15, 60), Vector3.new(0, 15, 0))

while task.wait(INTERVAL_SECONDS) do
	local block = Instance.new("Part")
	block.Anchored = false
	block.CanCollide = true
	block.Color = Color3.fromHSV(math.random(), 1, 1)
	block.Position = Vector3.new(0, 30, 0)
	block.Rotation = Vector3.new(math.random(0, 360), math.random(0, 360), math.random(0, 360))
	block.Size = Vector3.new(1, 1, 1)
	block.Parent = Workspace
end
