#include "gargantuan.core/classes/DataModel.hpp"
#include "gargantuan.core/datatypes/Instance.hpp"
#include "gargantuan.core/reflection/InstanceClassRegistry.hpp"
#include "gargantuan.core/services/ProcessService.hpp"
#include "gargantuan.core/services/RunService.hpp"
#include "gargantuan.core/services/TweenService.hpp"
#include "gargantuan.core/services/UserInputService.hpp"
#include "gargantuan.core/services/Workspace.hpp"

namespace gargantuan {
	G_INSTANCE_IMPL(
		DataModel,
		.Description = "The root hierachy and service provider for Gargantuan games.",
		.Superclass = "ServiceProvider"
	);

	const DataModel::ServiceDefinitions &DataModel::GetServiceDefinitions() const {
		static const DataModel::ServiceDefinitions CONSTRUCTORS = {
			{"ProcessService", ProcessService::CLASS_DEFINITION},
			{"RunService", RunService::CLASS_DEFINITION},
			{"TweenService", TweenService::CLASS_DEFINITION},
			{"UserInputService", UserInputService::CLASS_DEFINITION},
			{"Workspace", Workspace::CLASS_DEFINITION},
		};
		return CONSTRUCTORS;
	};
}
