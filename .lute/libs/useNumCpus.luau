local process = require("@std/process")

const function useNumCpus(): number
	-- Windows
	const envNumProcessors = process.env["NUMBER_OF_PROCESSORS"]
	if envNumProcessors then
		const numProcessors = tonumber(envNumProcessors)
		if numProcessors and numProcessors > 0 then
			return numProcessors
		end
	end

	-- Linux
	const nprocResult = process.run({ "nproc", "--all" })
	if nprocResult.ok then
		const numProcessors = tonumber(nprocResult.stdout)
		if numProcessors and numProcessors > 0 then
			return numProcessors
		end
	end

	-- MacOS
	const sysResult = process.run({ "sysctl", "-n", "hw.ncpu" })
	if sysResult.ok then
		const numProcessors = tonumber(sysResult.stdout)
		if numProcessors and numProcessors > 0 then
			return numProcessors
		end
	end

	return 1
end

return useNumCpus
