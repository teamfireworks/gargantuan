local uis = game:GetService("UserInputService")
local w = game:GetService("Workspace")

local connection: RBXScriptConnection

local function makeBaseplate()
	local baseplate = Instance.new("Part")
	baseplate.Anchored = true
	baseplate.Color = Color3.fromHSV(0, 0, 0.5)
	baseplate.Position = Vector3.new(0, -20, 0)
	baseplate.Size = Vector3.new(1000, 16, 1000)
	baseplate.Parent = w
end

local function weldConstraintScene()
	local part0 = Instance.new("Part")
	part0.Anchored = false
	part0.Color = Color3.fromRGB(150, 0, 0)
	part0.Position = Vector3.new(-5, 0, 0)
	part0.Size = Vector3.new(2, 2, 2)
	part0.Parent = w

	local part1 = Instance.new("Part")
	part1.Anchored = false
	part1.Color = Color3.fromRGB(0, 0, 150)
	part1.Position = Vector3.new(2, 1, 0)
	part1.Size = Vector3.new(2, 2, 2)
	part1.Parent = w

	local wc = Instance.new("WeldConstraint")
	wc.Part0 = part0
	wc.Part1 = part1
	wc.Enabled = true
	wc.Parent = w

	connection = uis.InputBegan:Connect(function(inp)
		if inp.KeyCode == Enum.KeyCode.E then
			part0:ApplyImpulse(Vector3.new(0, 500, 0))
		end
	end)
end

local function prismaticConstraintScene()
	local part0 = Instance.new("Part")
	part0.Anchored = true
	part0.Color = Color3.fromRGB(150, 0, 0)
	part0.Position = Vector3.new(-5, 0, 0)
	part0.Size = Vector3.new(2, 2, 2)
	part0.Parent = w

	local a0 = Instance.new("Attachment")
	a0.Parent = part0

	local part1 = Instance.new("Part")
	part1.Anchored = false
	part1.Color = Color3.fromRGB(0, 0, 150)
	part1.Position = Vector3.new(2, 1, 0)
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
	prisma.Speed = 50
	prisma.UpperLimit = 250
	prisma.Parent = w

	connection = uis.InputBegan:Connect(function(inp)
		if inp.KeyCode == Enum.KeyCode.E then
			part1:ApplyImpulse(Vector3.new(500, 0, 0))
		end
	end)
end

local function hingeConstraintScene()
	local part0 = Instance.new("Part")
	part0.Anchored = true
	part0.Color = Color3.fromRGB(150, 0, 0)
	part0.Position = Vector3.new(-5, 0, 0)
	part0.Size = Vector3.new(2, 2, 2)
	part0.Parent = w

	local attachmentRotation = CFrame.Angles(0, -math.pi / 2, 0)

	local a0 = Instance.new("Attachment")
	a0.CFrame = CFrame.new(0, 0, 1) * attachmentRotation
	a0.Parent = part0

	local part1 = Instance.new("Part")
	part1.Anchored = false
	part1.Color = Color3.fromRGB(0, 0, 150)
	part1.Position = Vector3.new(-5, 0, 2)
	part1.Size = Vector3.new(10, 2, 2)
	part1.Parent = w

	local a1 = Instance.new("Attachment")
	a1.CFrame = CFrame.new(0, 0, -1) * attachmentRotation
	a1.Parent = part1

	local hinge = Instance.new("HingeConstraint")
	hinge.Attachment0 = a0
	hinge.Attachment1 = a1
	hinge.Enabled = true
	hinge.ActuatorType = Enum.ActuatorType.Servo
	hinge.ServoMaxTorque = 1000
	hinge.AngularSpeed = 2
	hinge.TargetAngle = 0
	hinge.Parent = w

	connection = uis.InputBegan:Connect(function(inp)
		if inp.KeyCode == Enum.KeyCode.E then
			hinge.TargetAngle += 45
		end
	end)
end

local function springConstraintScene()
	local part0 = Instance.new("Part")
	part0.Anchored = true
	part0.Color = Color3.fromRGB(150, 0, 0)
	part0.Position = Vector3.new(0, 8, 0)
	part0.Size = Vector3.new(2, 2, 2)
	part0.Parent = w

	local a0 = Instance.new("Attachment")
	a0.CFrame = CFrame.new(0, -1, 0)
	a0.Parent = part0

	local part1 = Instance.new("Part")
	part1.Anchored = false
	part1.Color = Color3.fromRGB(0, 0, 150)
	part1.Position = Vector3.new(0, 0, 0)
	part1.Size = Vector3.new(2, 2, 2)
	part1.Parent = w

	local a1 = Instance.new("Attachment")
	a1.CFrame = CFrame.new(0, 1, 0)
	a1.Parent = part1

	local spring = Instance.new("SpringConstraint")
	spring.Attachment0 = a0
	spring.Attachment1 = a1
	spring.Enabled = true
	spring.FreeLength = 5
	spring.Stiffness = 300
	spring.Damping = 5
	spring.Parent = w

	connection = uis.InputBegan:Connect(function(inp)
		if inp.KeyCode == Enum.KeyCode.E then
			part1:ApplyImpulse(Vector3.new(10, -1000, 0))
		end
	end)
end

local scenes = {
	[Enum.KeyCode.One] = weldConstraintScene,
	[Enum.KeyCode.Two] = prismaticConstraintScene,
	[Enum.KeyCode.Three] = hingeConstraintScene,
	[Enum.KeyCode.Four] = springConstraintScene,
}

local readableScenes: { [string]: string } = {
	["1"] = "Weld Constraint Example",
	["2"] = "Prismatic Constraint Example",
	["3"] = "Hinge Constraint Example",
	["4"] = "Spring Constraint Example",
}

uis.InputBegan:Connect(function(inp)
	for key, func in pairs(scenes) do
		if key == inp.KeyCode then
			if connection then
				connection:Disconnect()
			end
			w:ClearAllChildren()
			makeBaseplate()
			func()
		end
	end
end)

print("=====================WELCOME TO THE CONSTRAINT EXAMPLES!=====================")
print("Scenes: ")
for key: string, title: string in pairs(readableScenes) do
	print(key .. ": " .. title)
end
print("You can interact with each scene by pressing E.")
makeBaseplate()
