-- this is supposed to be the master example
-- number keys go to different constraints
-- eg press 1 to run the weldconstraint test etc etc
-- will flesh out and make a better selector later
local uis = game:GetService("UserInputService")
local w = game:GetService("Workspace")

local connection: RBXScriptConnection --this is for interacting with the scene through jumprequest

local function makeBaseplate()
	local baseplate = Instance.new("Part")
	baseplate.Anchored = true
	baseplate.Color = Color3.fromHSV(0, 0, 0.5)
	baseplate.Position = Vector3.new(0, -8, 0)
	baseplate.Size = Vector3.new(1000, 16, 1000)
	baseplate.Parent = w
end

local function weldConstraintScene()
	local part0 = Instance.new("Part")
	part0.Anchored = false
	part0.Color = Color3.fromRGB(150, 0, 0) --red
	part0.Position = Vector3.new(-5, 0, 0)
	part0.Size = Vector3.new(2, 2, 2)
	part0.Parent = w

	local part1 = Instance.new("Part")
	part1.Anchored = false
	part1.Color = Color3.fromRGB(0, 0, 150) --blue
	part1.Position = Vector3.new(2, 1, 0)
	part1.Size = Vector3.new(2, 2, 2)
	part1.Parent = w

	local wc = Instance.new("WeldConstraint")
	wc.Part0 = part0
	wc.Part1 = part1
	wc.Enabled = true
	wc.Parent = w

	connection = uis.JumpRequest:Connect(function()
		part0:ApplyImpulse(Vector3.new(0, 5, 0))
	end)
end

local function prismaticConstraintScene()
	local part0 = Instance.new("Part")
	part0.Anchored = true
	part0.Color = Color3.fromRGB(150, 0, 0) --red
	part0.Position = Vector3.new(-5, 0, 0)
	part0.Size = Vector3.new(2, 2, 2)
	part0.Parent = w

	local a0 = Instance.new("Attachment")
	a0.Parent = part0

	local part1 = Instance.new("Part")
	part1.Anchored = false
	part1.Color = Color3.fromRGB(0, 0, 150) --bloo
	part1.Position = Vector3.new(2, 1, 0) --they start at different y positions, so the prismatic constraint should align them
	part1.Size = Vector3.new(2, 2, 2)
	part1.Parent = w

	local a1 = Instance.new("Attachment")
	a1.Parent = part1

	local prisma = Instance.new("PrismaticConstraint")
	prisma.Attachment0 = a0
	prisma.Attachment1 = a1
	prisma.Enabled = true
	prisma.LimitsEnabled = true
	prisma.ActuatorType = Enum.ActuatorType.Servo
	prisma.TargetPosition = 5
	prisma.UpperLimit = 10
	prisma.Parent = w

	connection = uis.JumpRequest:Connect(function()
		part1:ApplyImpulse(Vector3.new(5, 0, 0))
		--should move away from part0 then get pulled back
	end)
end

local function hingeConstraintScene()
	local part0 = Instance.new("Part")
	part0.Anchored = true
	part0.Color = Color3.fromRGB(150, 0, 0) --red
	part0.Position = Vector3.new(-5, 0, 0)
	part0.Size = Vector3.new(2, 2, 2)
	part0.Parent = w

	local a0 = Instance.new("Attachment")
	a0.CFrame = CFrame.new(0, 1, 0)
	a0.Parent = part0

	local part1 = Instance.new("Part")
	part1.Anchored = false
	part1.Color = Color3.fromRGB(0, 0, 150) --bloo
	part1.Position = Vector3.new(-5, 2, 0)
	part1.Size = Vector3.new(10, 2, 2)
	part1.Parent = w

	local a1 = Instance.new("Attachment")
	a1.CFrame = CFrame.new(0, -1, 0)
	a1.Parent = part1

	local hinge = Instance.new("HingeConstraint")
	hinge.Attachment0 = a0
	hinge.Attachment1 = a1
	hinge.Enabled = true
	hinge.ActuatorType = Enum.ActuatorType.Motor
	hinge.AngularVelocity = math.pi --1 full rotation every 2 seconds
	hinge.Parent = w

	connection = uis.JumpRequest:Connect(function()
		part1:ApplyAngularImpulse(Vector3.new(5, 0, 0)) -- idk if this works yet
	end)
end

local scenes = {
	[Enum.KeyCode.One] = weldConstraintScene,
	[Enum.KeyCode.Two] = prismaticConstraintScene,
	[Enum.KeyCode.Three] = hingeConstraintScene,
}

local readableScenes: { [string]: string } = {
	["1"] = "Weld Constraint Example",
	["2"] = "Prismatic Constraint Example",
	["3"] = "Hinge Constraint Example",
}

uis.InputBegan:Connect(function(inp)
	if scenes[inp.KeyCode] then
		connection:Disconnect()
		w:ClearAllChildren()
		makeBaseplate()
		w.CurrentCamera.CameraType = Enum.CameraType.Scriptable
		w.CurrentCamera.CFrame = CFrame.new(0, 0, 0)
		w.CurrentCamera.CameraType = Enum.CameraType.Custom
		scenes[inp.KeyCode]()
	end
end)

print("=====================WELCOME TO THE CONSTRAINT EXAMPLES!=====================")
print("Scenes: ")
for key: string, title: string in pairs(readableScenes) do
	print(key .. ": " .. title)
end
print("You can interact with each scene by pressing space.")
