--!nocheck

local ProcessService = game:GetService("ProcessService")

print("Process start")

task.wait(3)

print("Exiting")
ProcessService:ExitAsync(1)

print("This will not run")
