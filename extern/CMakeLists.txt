# Copyright 2026 Brooke Nguyen o/b/o Team Fireworks Games
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

set(FLECS_STATIC ON CACHE BOOL "Build Flecs as a static library" FORCE)
set(GLM_ENABLE_CXX_20 ON CACHE BOOL "Enable C++20 features for GLM" FORCE)
set(LUAU_BUILD_CLI OFF CACHE BOOL "Do not build the Luau CLI" FORCE)
set(LUAU_BUILD_TESTS OFF CACHE BOOL "Do not build the Luau test suite" FORCE)
set(LUAU_BUILD_WEB OFF CACHE BOOL "Do not build the Luau web module" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build SDL3 as a static library" FORCE)

add_subdirectory(box3d SYSTEM EXCLUDE_FROM_ALL)
add_subdirectory(flecs SYSTEM EXCLUDE_FROM_ALL)
add_subdirectory(glaze SYSTEM EXCLUDE_FROM_ALL)
add_subdirectory(glm SYSTEM EXCLUDE_FROM_ALL)
add_subdirectory(Luau SYSTEM EXCLUDE_FROM_ALL)
add_subdirectory(magic_enum SYSTEM EXCLUDE_FROM_ALL)
# add_subdirectory(msdf_atlas_gen SYSTEM EXCLUDE_FROM_ALL)
add_subdirectory(SDL3 SYSTEM EXCLUDE_FROM_ALL)
add_subdirectory(SDL3_image SYSTEM EXCLUDE_FROM_ALL)
add_subdirectory(SDL3_ttf SYSTEM EXCLUDE_FROM_ALL)
add_subdirectory(Tracy SYSTEM EXCLUDE_FROM_ALL)
