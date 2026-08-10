local RunService = game:GetService("RunService")
local Workspace = game:GetService("Workspace")

const GRID_SIZE = 20
const BLOCK_SIZE = 2
const SCROLL_SPEED = 0.5
const Y_OFFSET = 3

local COLOR_STOPS = {
	Color3.fromRGB(180, 0, 0),
	Color3.fromRGB(255, 70, 0),
	Color3.fromRGB(255, 180, 0),
	Color3.fromRGB(255, 230, 120),
	Color3.fromRGB(255, 255, 255),
	Color3.fromRGB(255, 230, 120),
	Color3.fromRGB(255, 180, 0),
	Color3.fromRGB(255, 70, 0),
}

local blocks = {}
local t = 0

for x = 1, GRID_SIZE do
	blocks[x] = {}
	for z = 1, GRID_SIZE do
		local p = Instance.new("Part")
		p.Anchored = true
		p.CFrame = CFrame.new((x - (GRID_SIZE + 1) / 2) * BLOCK_SIZE, Y_OFFSET, (z - (GRID_SIZE + 1) / 2) * BLOCK_SIZE)
		p.Size = Vector3.new(BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE)
		p.Parent = Workspace
		blocks[x][z] = p
	end
end

local function palette(u: number)
	u = u % 1
	local scaled = u * #COLOR_STOPS
	local i = math.floor(scaled)
	local f = scaled - i
	local c1 = COLOR_STOPS[(i % #COLOR_STOPS) + 1]
	local c2 = COLOR_STOPS[((i + 1) % #COLOR_STOPS) + 1]
	return c1:Lerp(c2, f)
end

local function field(x: number, z: number)
	local wx = (x - (GRID_SIZE + 1) / 2) * BLOCK_SIZE
	local wz = (z - (GRID_SIZE + 1) / 2) * BLOCK_SIZE

	local wavelength = 20
	return (wx + wz) / wavelength + t * SCROLL_SPEED
end

RunService.PreRender:Connect(function(dt)
	t = t + dt
	for x = 1, GRID_SIZE do
		for z = 1, GRID_SIZE do
			local c = palette(field(x, z))
			local p = blocks[x][z]
			p.Color = c
			local y = Y_OFFSET + c.G * 4.5
			p.CFrame = CFrame.new((x - (GRID_SIZE + 1) / 2) * BLOCK_SIZE, y, (z - (GRID_SIZE + 1) / 2) * BLOCK_SIZE)
		end
	end
end)
