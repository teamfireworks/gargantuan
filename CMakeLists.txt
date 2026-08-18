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

add_subdirectory(extern)

# Libraries
add_subdirectory(src/gargantuan_core)
add_subdirectory(src/gargantuan_datatypes)
add_subdirectory(src/gargantuan_instances)
add_subdirectory(src/gargantuan_renderer)
add_subdirectory(src/gargantuan_runtime)

# Executables
add_subdirectory(src/gargantuan_editor)
add_subdirectory(src/gargantuan_runner)
