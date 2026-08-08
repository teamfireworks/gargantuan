#include "gargantuan/classes/DirectoryLink.hpp"
#include "gargantuan/Log.hpp"
#include "gargantuan/classes/Folder.hpp"
#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/classes/LuaSourceContainer.hpp"
#include "gargantuan/classes/ModuleScript.hpp"
#include "gargantuan/classes/Script.hpp"
#include "gargantuan/filesystem/Paths.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_filesystem.h>
#include <exception>
#include <filesystem>
#include <memory>
#include <string>

namespace gargantuan {
	G_INSTANCE_IMPL(
		DirectoryLink,
		.Description = "Synchronizes a filesystem entry into the data model.",
		.Properties = {
			{"Path", Property::fromMember<&DirectoryLink::Path>(true, true).SetSerializable()},
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
				auto script = ScriptFromFile<T>(Paths::ToUtf8(absolutePath).c_str());
				script->Name = filename.substr(0, filename.size() - extension.size());
				return script;
			} catch (std::exception &err) {
				LOG_WARN(
					App,
					"Failed to create %s %s: %s",
					debugNoun.c_str(),
					Paths::ToUtf8(absolutePath).c_str(),
					err.what()
				);
			}
		}
		return nullptr;
	}

	std::shared_ptr<Instance> InstanceFromPath(const std::filesystem::path absolutePath) {
		SDL_PathInfo pathInfo;
		if (!SDL_GetPathInfo(Paths::ToUtf8(absolutePath).c_str(), &pathInfo)) {
			LOG_WARN(App, "Failed to synchronize %s: %s", Paths::ToUtf8(absolutePath).c_str(), SDL_GetError());
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
			container->Name = absolutePath.filename().string();
			for (const auto &entry : std::filesystem::directory_iterator(absolutePath)) {
				auto child = InstanceFromPath(entry.path());
				if (!child) continue;
				child->SetParent(container);
			}
			return container;
		}

		return nullptr;
	}

	void DirectoryLink::Synchronize(const std::filesystem::path absolutePath) {
		if (!Parent || Synchronizing) return;
		Synchronizing = true;

		LOG_INFO(App, "Synchronizing DirectoryLink path: %s", Paths::ToUtf8(absolutePath).c_str());

		for (auto &child : OwnedSiblings) {
			child->Destroy();
		}
		OwnedSiblings.clear();

		SDL_PathInfo pathInfo;
		if (!SDL_GetPathInfo(Paths::ToUtf8(absolutePath).c_str(), &pathInfo)) {
			LOG_WARN(
				App, "Failed to get path information for %s: %s", Paths::ToUtf8(absolutePath).c_str(), SDL_GetError()
			);
			return;
		} else if (pathInfo.type != SDL_PATHTYPE_DIRECTORY) {
			LOG_WARN(App, "DirectoryLinks (for now) can only be used with directories");
			return;
		};

		for (const auto &entry : std::filesystem::directory_iterator(absolutePath)) {
			auto child = InstanceFromPath(entry.path());
			if (!child) continue;
			child->Archivable = false;
			child->SetParent(Parent->shared_from_this());
			OwnedSiblings.push_back(child);
		}

		Synchronizing = false;
	};
}
