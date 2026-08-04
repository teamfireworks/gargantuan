#include "gargantuan.core/filesystem/Paths.hpp"
#include <SDL3/SDL.h>
#include <filesystem>

namespace gargantuan::Paths {
	std::filesystem::path GetExecutableDirectory() {
		const char *baseDirectory = SDL_GetBasePath();
		if (!baseDirectory) {
			return std::filesystem::current_path();
		};
		return baseDirectory;
	}
}
