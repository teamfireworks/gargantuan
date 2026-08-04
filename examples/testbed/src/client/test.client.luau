print("there is no fucking shot this works")

task.delay(1, function()
	for _, v in game:GetDescendants() do
		print(v:GetFullName())
	end
end)
