cmake_minimum_required(VERSION 3.25)

project(
	gargantuan
				VERSION ${LUTE_VERSION}
				LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(vendor)
add_subdirectory(gargantuan.core)
add_subdirectory(gargantuan.editor)
add_subdirectory(gargantuan.runtime)
