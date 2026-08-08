// Copyright (c) 2026 Team Fireworks
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/classes/generated/DirectoryLink.hpp"

#include <filesystem>
#include <vector>

namespace gargantuan {
	class DirectoryLink : public Instance {
		G_DECL_DIRECTORYLINK;

		std::vector<std::shared_ptr<Instance>> OwnedSiblings;
		bool Synchronizing = false;

		// TODO: This syncs on engine start up. We should implement file
		// watching. Also use the Filesystem classes
		void Synchronize(const std::filesystem::path absolutePath);
	};
}
