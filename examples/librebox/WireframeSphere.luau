-- Creates a rotating wireframe sphere with latitude and longitude rings.

local Workspace = game:GetService("Workspace")
local RunService = game:GetService("RunService")

local CENTER = Vector3.new(0, 20, 0)
local COLOR = Color3.fromRGB(255, 255, 0)
local DEGREES_PER_SECOND = 20
local RADIUS = 15
local SEGMENTS = 16
local THICKNESS = 0.3
local TRANSPARENCY = 0

local baseplate = Instance.new("Part")
baseplate.Name = "Baseplate"
baseplate.Anchored = true
baseplate.Color = Color3.fromHSV(0, 0, 0.5)
baseplate.Size = Vector3.new(512, 20, 512)
baseplate.Position = Vector3.new(0, -20, 0)
baseplate.Parent = Workspace

local function createPart()
	local part = Instance.new("Part")
	part.Anchored = true
	part.CanCollide = false
	part.Size = Vector3.new(THICKNESS, THICKNESS, 1)
	part.Color = COLOR
	part.Transparency = TRANSPARENCY
	part.Parent = Workspace
	return part
end

local parts: { Part } = {}
local relativeCFrames: { CFrame } = {}
local centerCFrame = CFrame.new(CENTER)

-- Helper to connect two points with a part and cache local-space transform
local function connectPoints(p1: Vector3, p2: Vector3)
	local dir = p2 - p1
	if dir.Magnitude <= 1e-6 then
		return
	end
	local mid = (p1 + p2) / 2
	local part = createPart()
	part.Size = Vector3.new(THICKNESS, THICKNESS, dir.Magnitude)
	part.CFrame = CFrame.new(mid, p2)
	table.insert(parts, part)
	table.insert(relativeCFrames, centerCFrame:ToObjectSpace(part.CFrame) :: CFrame)
end

-- Longitude (vertical great circles)
for i = 0, SEGMENTS - 1 do
	local phi = 2 * math.pi * i / SEGMENTS
	for j = 0, SEGMENTS - 1 do
		local theta1 = math.pi * j / SEGMENTS
		local theta2 = math.pi * (j + 1) / SEGMENTS

		local p1 = Vector3.new(
			RADIUS * math.sin(theta1) * math.cos(phi),
			RADIUS * math.cos(theta1),
			RADIUS * math.sin(theta1) * math.sin(phi)
		) + CENTER

		local p2 = Vector3.new(
			RADIUS * math.sin(theta2) * math.cos(phi),
			RADIUS * math.cos(theta2),
			RADIUS * math.sin(theta2) * math.sin(phi)
		) + CENTER

		connectPoints(p1, p2)
	end
end

-- Latitude (horizontal rings)
for j = 1, SEGMENTS - 1 do
	local theta = math.pi * j / SEGMENTS
	for i = 0, SEGMENTS - 1 do
		local phi1 = 2 * math.pi * i / SEGMENTS
		local phi2 = 2 * math.pi * (i + 1) / SEGMENTS

		local p1 = Vector3.new(
			RADIUS * math.sin(theta) * math.cos(phi1),
			RADIUS * math.cos(theta),
			RADIUS * math.sin(theta) * math.sin(phi1)
		) + CENTER

		local p2 = Vector3.new(
			RADIUS * math.sin(theta) * math.cos(phi2),
			RADIUS * math.cos(theta),
			RADIUS * math.sin(theta) * math.sin(phi2)
		) + CENTER

		connectPoints(p1, p2)
	end
end

-- Rigid rotation about center
local angle = 0
RunService.PreRender:Connect(function(dt)
	angle += dt * math.rad(DEGREES_PER_SECOND)
	local spin = centerCFrame * CFrame.Angles(angle * 0.5, angle, 0)
	for i = 1, #parts do
		parts[i].CFrame = spin * relativeCFrames[i]
	end
end)
