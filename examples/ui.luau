const CoreGui = game:GetService("CoreGui")

CoreGui.Name = "CoreGui"

const dialog = Instance.new("Frame")
dialog.BackgroundColor3 = Color3.new(1, 1, 1)
dialog.Size = UDim2.fromScale(1, 0.5)
dialog.Name = "Dialog"
dialog.BackgroundTransparency = 0

const text = Instance.new("TextLabel")
text.Text = "Kris, when I cast Snowgrave..."
text.Size = UDim2.fromScale(1, 1)
text.Name = "Text"
text.BackgroundTransparency = 1

const screen = Instance.new("ScreenGui")
text.Parent = dialog
dialog.Parent = screen
screen.Parent = CoreGui
