#pragma once

#include "gargantuan/services/generated/ProcessService.hpp"

namespace gargantuan {
	class ProcessService : public Instance {
		I_ProcessService;

		bool Alive = false;
		int ExitCode = 0;

		void CExitAsync(int exitCode);
	};
}
