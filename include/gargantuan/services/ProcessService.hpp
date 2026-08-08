#pragma once

#include "gargantuan/classes/Instance.hpp"

namespace gargantuan {
	class ProcessService : public Instance {
	  public:
		G_INSTANCE_DECL(ProcessService);

		bool Alive = false;
		int ExitCode = 0;

		void ExitAsync(int exitCode);
		static int LExitAsync(lua_State *L, Instance *self);
		static int LWriteToStdout(lua_State *L, Instance *self);
	};
}
