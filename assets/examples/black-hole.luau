local RunService = game:GetService("RunService")
local Workspace = game:GetService("Workspace")

type Disk = {
	part: BasePart,
	angle: number,
	radius: number,
	height: number,
	speed: number,
}

local center = Vector3.new(0, 10, 0)

local blackHole = Instance.new("Part")
blackHole.Size = Vector3.new(8, 8, 8)
blackHole.Anchored = true
blackHole.CanCollide = false
blackHole.Shape = Enum.PartType.Ball
-- blackHole.Material = Enum.Material.Neon
blackHole.Color = Color3.new(0, 0, 0)
blackHole.CFrame = CFrame.new(center)
blackHole.Parent = Workspace

local diskParts: { Disk } = {}
local numDisks = 1000
local minR, maxR = 9, 20

for i = 1, numDisks do
	local angle = math.random() * math.pi * 2
	local r = math.random(minR * 10, maxR * 10) / 10 -- more precise radius
	local y = math.random(-30, 30) / 10 * (1 - (r - minR) / (maxR - minR) * 0.6)
	local x = math.cos(angle) * r
	local z = math.sin(angle) * r

	local disk = Instance.new("Part")
	disk.Size = Vector3.new(0.5, 0.5, 0.5)
	disk.Anchored = true
	disk.CanCollide = false
	disk.Shape = Enum.PartType.Ball
	-- disk.Material = Enum.Material.Neon
	disk.Color = Color3.fromHSV(math.random(), 1, 1) -- full range for more color variety
	disk.CFrame = CFrame.new(center + Vector3.new(x, y, z))
	disk.Parent = Workspace

	table.insert(diskParts, {
		part = disk,
		angle = angle,
		radius = r,
		height = y,
		speed = 0.2 + math.random() * 1.0, -- varied rotation speed
	})
end

RunService.PreRender:Connect(function(dt: number)
	for _, data in pairs(diskParts) do
		data.angle = data.angle + data.speed * dt
		local x = math.cos(data.angle) * data.radius
		local z = math.sin(data.angle) * data.radius
		data.part.CFrame = CFrame.new(center + Vector3.new(x, 0, z))
	end
end)
