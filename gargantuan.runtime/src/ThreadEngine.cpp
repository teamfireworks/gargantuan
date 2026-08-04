#include "gargantuan.runtime/ThreadEngine.hpp"

#include <SDL3/SDL_log.h>
#include <chrono>
#include <lua.h>

namespace gargantuan {
	double GetCurrentTime() {
		return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
	};

	ThreadEngine::ThreadEngine(lua_State *mainState) : L(mainState) {};

	int ThreadEngine::TakeThreadReference(lua_State *thread) {
		lua_pushthread(thread);
		lua_xmove(thread, L, 1);
		int reference = lua_ref(L, -1);
		// lua_ref keeps the value on the stack
		lua_pop(L, 1);
		return reference;
	}

	void ThreadEngine::ResumeThread(lua_State *thread, int threadReference, int argumentCount) {
		int status = lua_resume(thread, L, argumentCount);
		switch (status) {
		case LUA_YIELD:
			break;

		case LUA_OK:
			lua_unref(L, threadReference);
			break;

		default:
			lua_unref(L, threadReference);
			SDL_Log("Thread error: %s", lua_tostring(thread, -1));
		}
	}

	void ThreadEngine::Step() {
		auto currentTime = GetCurrentTime();
		while (!ScheduledQueue.empty() && ScheduledQueue.top().WakeTime <= currentTime) {
			auto task = ScheduledQueue.top();
			ScheduledQueue.pop();

			switch (task.type) {
			case ThreadEngine::ScheduledTask::Type::Delay: {
				ResumeThread(task.Thread, task.ThreadReference, task.ArgumentCount);
				break;
			}
			case ThreadEngine::ScheduledTask::Type::Wait: {
				double actualWait = currentTime - task.ScheduledTime;
				lua_pushnumber(task.Thread, actualWait);
				ResumeThread(task.Thread, task.ThreadReference, 1);
				break;
			}
			}
		}

		while (!DeferredQueue.empty()) {
			std::vector<DeferredTask> currentBatch;
			currentBatch.swap(DeferredQueue);

			for (auto &task : currentBatch) {
				ResumeThread(task.Thread, task.ThreadReference, task.ArgumentCount);
			}
		}
	}

	void ThreadEngine::QueueScheduledTask(
		lua_State *thread, ScheduledTask::Type type, double delaySeconds, int argumentCount
	) {
		ScheduledQueue.push({
			.type = type,
			.Thread = thread,
			.ThreadReference = TakeThreadReference(thread),
			.ArgumentCount = argumentCount,
			.ScheduledTime = GetCurrentTime(),
			.WakeTime = GetCurrentTime() + delaySeconds,
		});
	}

	void ThreadEngine::QueueDeferredTask(lua_State *thread, int argumentCount) {
		DeferredQueue.push_back({
			.Thread = thread,
			.ThreadReference = TakeThreadReference(thread),
			.ArgumentCount = argumentCount,
		});
	}
} // namespace gargantuan
