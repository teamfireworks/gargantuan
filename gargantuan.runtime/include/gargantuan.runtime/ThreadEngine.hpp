#pragma once

#include <lua.h>
#include <queue>
#include <vector>

namespace gargantuan {
	class ThreadEngine {
	  public:
		struct ScheduledTask {
			enum class Type {
				// task.wait()
				Wait,
				// task.delay()
				Delay,
			};

			Type type = Type::Wait;
			lua_State *Thread = nullptr;
			int ThreadReference = LUA_NOREF;
			int ArgumentCount = 0;
			double ScheduledTime = 0;
			double WakeTime = 0;
		};

		struct DeferredTask {
			lua_State *Thread = nullptr;
			int ThreadReference = LUA_NOREF;
			int ArgumentCount = 0;
		};

		ThreadEngine(lua_State *mainState);
		int TakeThreadReference(lua_State *thread);
		void Step();
		void ResumeThread(lua_State *thread, int threadReference, int argumentCount);
		void QueueScheduledTask(lua_State *thread, ScheduledTask::Type type, double delaySeconds, int argumentCount);
		void QueueDeferredTask(lua_State *thread, int argumentCount);

	  private:
		lua_State *L;

		struct CompareWakeTime {
			bool operator()(const ScheduledTask &lhs, const ScheduledTask &rhs) {
				return lhs.WakeTime > rhs.WakeTime;
			};
		};

		std::priority_queue<ScheduledTask, std::vector<ScheduledTask>, CompareWakeTime> ScheduledQueue;
		std::vector<DeferredTask> DeferredQueue;
	};
} // namespace gargantuan
