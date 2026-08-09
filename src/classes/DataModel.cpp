#include "gargantuan/classes/DataModel.hpp"

#include "gargantuan/services/ProcessService.hpp"
#include "gargantuan/services/RunService.hpp"
#include "gargantuan/services/UserInputService.hpp"
#include "gargantuan/services/Workspace.hpp"

namespace gargantuan {
	const DataModel::ServiceDefinitions &DataModel::GetServiceDefinitions() const {
		static const DataModel::ServiceDefinitions CONSTRUCTORS = {
			{"ProcessService", ProcessService::CLASS_DEFINITION},
			{"RunService", RunService::CLASS_DEFINITION},
			{"UserInputService", UserInputService::CLASS_DEFINITION},
			{"Workspace", Workspace::CLASS_DEFINITION},
		};
		return CONSTRUCTORS;
	};
}
