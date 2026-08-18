local Workspace = game:GetService("Workspace")
local UserInputService = game:GetService("UserInputService")

local baseplate = Instance.new("Part")
baseplate.Anchored = true
baseplate.Color = Color3.fromHSV(0, 0, 0.5)
baseplate.Position = Vector3.new(0, -8, 0)
baseplate.Size = Vector3.new(1000, 16, 1000)
baseplate.Parent = Workspace

local part0 = Instance.new("Part")
part0.Anchored = true
part0.Color = Color3.fromRGB(150,0,0) --red
part0.Position = Vector3.new(-5, 0, 0)
part0.Size = Vector3.new(2, 2, 2)
part0.Parent = Workspace

local a0 = Instance.new("Attachment")
a0.Parent = part0

local part1 = Instance.new("Part")
part1.Anchored = false
part1.Color = Color3.fromRGB(0,0,150) --bloo
part1.Position = Vector3.new(2, 1, 0) --they start at different y positions, so the prismatic constraint should align them
part1.Size = Vector3.new(2, 2, 2)
part1.Parent = Workspace

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
prisma.Parent = Workspace

UserInputService.JumpRequest:Connect(function()
	part1:ApplyImpulse(Vector3.new(5,0,0))
	--should move away from part0 then get pulled back
end)
