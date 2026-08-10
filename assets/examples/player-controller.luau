local RunService = game:GetService("RunService")
local UserInputService = game:GetService("UserInputService")
local Workspace = game:GetService("Workspace")

local baseplate = Instance.new("Part")
baseplate.Name = "Baseplate"
baseplate.Anchored = true
baseplate.Color = Color3.fromHSV(0, 0, 0.5)
baseplate.Position = Vector3.new(0, -10, 0)
baseplate.Size = Vector3.new(2000, 20, 2000)
baseplate.Transparency = 0
baseplate.Parent = Workspace

local hrp = Instance.new("Part")
hrp.Name = "HumanoidRootPart"
hrp.Anchored = true
hrp.Size = Vector3.new(2, 4, 1)
hrp.Position = Vector3.new(0, 2, 0)
hrp.Color = Color3.new(1, 1, 1)
hrp.Parent = Workspace

do
	local part = Instance.new("Part")
	part.Anchored = true
	part.Color = Color3.fromHSV(1, 0.5, 1)
	part.Position = Vector3.new(0, 2.5, 10)
	part.Parent = Workspace
	part.Size = Vector3.new(5, 5, 5)

	local wedge = Instance.new("Part")
	wedge.Anchored = true
	wedge.Color = Color3.fromHSV(0.3, 0.5, 1)
	wedge.CFrame = CFrame.new(Vector3.new(5, 1, -5), Vector3.new(10, 1, -5))
	wedge.Parent = Workspace
	wedge.Shape = Enum.PartType.Wedge
	wedge.Size = Vector3.new(5, 2, 2)
end

Workspace.CurrentCamera.CameraType = Enum.CameraType.Scriptable
Workspace.CurrentCamera.CFrame = CFrame.new((hrp.CFrame * CFrame.new(0, 20, 0)).Position, hrp.Position)

RunService.PreRender:Connect(function(dt: number)
	local walkDirection = Vector3.zero
	if UserInputService:IsKeyDown(Enum.KeyCode.W) then
		walkDirection += Vector3.new(0, 0, 0.5)
	end
	if UserInputService:IsKeyDown(Enum.KeyCode.S) then
		walkDirection -= Vector3.new(0, 0, 0.5)
	end
	if UserInputService:IsKeyDown(Enum.KeyCode.A) then
		walkDirection += Vector3.new(0.5, 0, 0)
	end
	if UserInputService:IsKeyDown(Enum.KeyCode.D) then
		walkDirection -= Vector3.new(0.5, 0, 0)
	end

	walkDirection *= 16 * dt
	hrp.CFrame = CFrame.new(hrp.Position + walkDirection, hrp.Position)

	local lookCFrame = CFrame.new((hrp.CFrame * CFrame.new(0, 20, 0)).Position, hrp.Position)
	Workspace.CurrentCamera.CFrame = Workspace.CurrentCamera.CFrame:Lerp(lookCFrame, dt * 2)
end)
