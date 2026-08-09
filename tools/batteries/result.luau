export type Result<Value> = | { success: true, value: Value } | { success: false, traceback: string, err: string }

local result = {}

function result.ok<Value>(value: Value): Result<Value>
	return {
		success = true,
		value = value,
	}
end

function result.fail<Value>(err: string): Result<any>
	-- Ignore the current call to debug.traceback, and the call to fail.
	local traceback = debug.traceback(nil, 2)

	return {
		success = false,

		traceback = traceback,
		err = err,
	}
end

function result.pcall<R, T...>(f: (T...) -> R, ...: T...): Result<R>
	local success, value = pcall(f, ...)

	if success then
		return result.ok(value)
	else
		local err = value :: any

		return result.fail(err)
	end
end

return result
