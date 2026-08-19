# Copyright 2026 Brooke Nguyen o/b/o Team Fireworks Games
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

file(GLOB_RECURSE GARGANTUAN_RUNNER_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")

add_executable(gargantuan_runner ${GARGANTUAN_RUNNER_SOURCES})

target_include_directories(gargantuan_runner PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(gargantuan_runner PRIVATE gargantuan::core)
