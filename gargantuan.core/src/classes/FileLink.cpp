#include "gargantuan.core/classes/FileLink.hpp"
#include "gargantuan.core/Log.hpp"
#include "gargantuan.core/classes/Folder.hpp"
#include "gargantuan.core/classes/LuaSourceContainer.hpp"
#include "gargantuan.core/classes/ModuleScript.hpp"
#include "gargantuan.core/classes/Script.hpp"
#include "gargantuan.core/datatypes/Instance.hpp"
#include "gargantuan.core/reflection/InstanceClassRegistry.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_filesystem.h>
#include <exception>
#include <filesystem>
#include <memory>
#include <string>

namespace gargantuan {
	G_INSTANCE_IMPL(
		FileLink,
		.Description = "Synchronizes a filesystem entry into the data model.",
		.Properties = {
			{"Path", Property::fromMember<&FileLink::Path>(true, true).SetSerializable()},
		}
	);

	template <typename T>
	std::shared_ptr<T> TryCreateScript(
		const std::string &extensionSuffix,
		const std::string &debugNoun,
		const std::string &filename,
		const std::filesystem::path &absolutePath
	) {
		auto extension = extensionSuffix + ".luau";
		if (filename.ends_with(extension)) {
			try {
				auto script = ScriptFromFile<T>(absolutePath.c_str());
				script->Name = filename.substr(0, filename.size() - extension.size());
				return script;
			} catch (std::exception &err) {
				LOG_WARN(App, "Failed to create %s %s: %s", debugNoun.c_str(), absolutePath.c_str(), err.what());
			}
		}
		return nullptr;
	}

	Instance::Pointer InstanceFromPath(const std::filesystem::path absolutePath) {
		SDL_PathInfo pathInfo;
		if (!SDL_GetPathInfo(absolutePath.c_str(), &pathInfo)) {
			LOG_WARN(App, "Failed to synchronize %s: %s", absolutePath.c_str(), SDL_GetError());
			return nullptr;
		};

		if (pathInfo.type == SDL_PATHTYPE_FILE) {
			auto filename = absolutePath.filename().string();
			if (filename.ends_with(".instance.json")) {
				// FIXME: self recursion
				// std::ifstream input(absolutePath.string());
				// auto state = InstanceSerialization::Deserialize(InstanceSerialization::InstanceFormat::Json, input);
				// if (state.Ok) {
				// 	return state.Instance;
				// } else {
				// 	LOG_WARN(App, "Failed to deserialize %s:", absolutePath.c_str());
				// 	for (auto &error : state.Errors) {
				// 		LOG_WARN(App, "* %s", error.c_str());
				// 	}
				// 	return nullptr;
				// }
				return nullptr;
			} else if (auto script = TryCreateScript<Script>(".client", "client script", filename, absolutePath)) {
				script->RunContext = Enums::RunContext::Client;
				return script;
			} else if (auto script = TryCreateScript<Script>(".server", "server script", filename, absolutePath)) {
				script->RunContext = Enums::RunContext::Server;
				return script;
			} else if (auto script = TryCreateScript<ModuleScript>("", "module script", filename, absolutePath)) {
				return script;
			}
		} else if (pathInfo.type == SDL_PATHTYPE_DIRECTORY) {
			auto container = std::make_shared<Folder>();
			container->Name = absolutePath.filename();
			for (const auto &entry : std::filesystem::directory_iterator(absolutePath)) {
				auto child = InstanceFromPath(entry.path());
				if (!child) continue;
				child->SetParent(container);
			}
			return container;
		}

		return nullptr;
	}

	void FileLink::Synchronize(const std::filesystem::path absolutePath) {
		if (!Parent || Synchronizing) return;
		Synchronizing = true;

		LOG_INFO(App, "Synchronizing FileLink path: %s", absolutePath.c_str());

		for (auto &child : Parent->GetChildren()) {
			if (child.get() != this) child->Destroy();
		}

		SDL_PathInfo pathInfo;
		if (!SDL_GetPathInfo(absolutePath.c_str(), &pathInfo)) {
			LOG_WARN(App, "Failed to get path information for %s: %s", absolutePath.c_str(), SDL_GetError());
			return;
		} else if (pathInfo.type != SDL_PATHTYPE_DIRECTORY) {
			LOG_WARN(App, "FileLinks (for now) can only be used with directories");
			return;
		};

		for (const auto &entry : std::filesystem::directory_iterator(absolutePath)) {
			auto child = InstanceFromPath(entry.path());
			if (!child) continue;
			child->Archivable = false;
			child->SetParent(Parent->shared_from_this());
			LOG_INFO(App, "Got %s", child->GetFullName().c_str());
		}

		Synchronizing = false;
	};
}
