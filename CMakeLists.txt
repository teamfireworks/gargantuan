# Copyright 2026 Brooke Nguyen o/b/o Team Fireworks Games
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF) # -std=c++23 instead of -std=gnu++23 in macOS
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GARGANTUAN_VERSION 0.1.0)
project(Gargantuan
	VERSION ${GARGANTUAN_VERSION}
	SPDX_LICENSE MPL-2.0
	DESCRIPTION "An independent game engine for Roblox developers"
	HOMEPAGE_URL "https://gargantuan.teamfireworks.org/"
	LANGUAGES CXX
)

set(GARGANTUAN_VERSION_SUFFIX_DEFAULT "$ENV{GARGANTUAN_VERSION_SUFFIX}")
if(
	NOT DEFINED GARGANTUAN_VERSION_SUFFIX
	AND NOT "${GARGANTUAN_VERSION_SUFFIX_DEFAULT}" STREQUAL ""
)
	set(GARGANTUAN_VERSION_SUFFIX "${GARGANTUAN_VERSION_SUFFIX_DEFAULT}")
endif()
set(
	GARGANTUAN_VERSION_SUFFIX
	"${GARGANTUAN_VERSION_SUFFIX}"
	CACHE STRING
	"Optional suffix appended to the Gargantuan version string"
)
set(GARGANTUAN_VERSION_FULL "${GARGANTUAN_VERSION}")
if(GARGANTUAN_VERSION_SUFFIX)
	set(
		GARGANTUAN_VERSION_FULL
		"${GARGANTUAN_VERSION_FULL}-${GARGANTUAN_VERSION_SUFFIX}"
	)
endif()
message("${GARGANTUAN_VERSION_FULL}")

if(
	NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extern/msdf_atlas_gen/CMakeLists.txt"
	# OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/extern/msdf_atlas_gen/msdfgen/CMakeLists.txt"
)
	message(FATAL_ERROR
				"Your Gargantuan repository looks incomplete. Please read the following "
				"instructions carefully:"
				"\n"
				"1) If you have downloaded Aseprite source using the "
				"\"Code > Download ZIP\" button from GitHub, you will not be able to "
				"compile Gargantuan. You need to get the code with other methods (continue "
				"reading)."
				"\n"
				"2) You can correctly clone the GitHub repository with the "
				"\"--recursive\" argument:"
				"\n"
				"  git clone --recursive https://github.com/teamfireworks/gargantuan.git"
				"\n"
				"3) If you have already cloned the repository and see this message, "
				"you still need to initialize submodules running the following command:"
				"\n"
				"  git submodule update --init --recursive"
				"\n"
				"More information in the documentation:"
				"\n"
				"  https://gargantuan.teamfireworks.org/developing/build-system")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(extern)

# Libraries
add_subdirectory(src/core)
add_subdirectory(src/renderer)
add_subdirectory(src/runtime)

# Executables
add_subdirectory(src/editor)
add_subdirectory(src/runner)
