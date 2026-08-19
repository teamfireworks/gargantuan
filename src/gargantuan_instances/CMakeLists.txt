# Copyright 2026 Brooke Nguyen o/b/o Team Fireworks Games
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

file(GLOB_RECURSE GARGANTUAN_INSTANCES_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")

add_library(gargantuan_instances STATIC ${GARGANTUAN_INSTANCES_SOURCES})
add_library(gargantuan::instances ALIAS gargantuan_instances)

target_include_directories(gargantuan_instances PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(gargantuan_instances
	PRIVATE
		box3d::box3d
		flecs_static
		glaze::glaze
		glm
		Luau.CodeGen
		Luau.Compiler
		Luau.Config
		Luau.Require
		Luau.VM
		SDL3::SDL3
		SDL3_image-static
		SDL3_ttf-static
		Tracy::TracyClient
)
