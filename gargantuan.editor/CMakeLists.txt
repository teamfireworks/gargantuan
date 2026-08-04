cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE GARGANTUAN_SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")
add_executable(gargantuan.editor ${GARGANTUAN_SOURCES})
set_target_properties(gargantuan.editor PROPERTIES OUTPUT_NAME lute)
target_link_libraries(gargantuan.editor PRIVATE gargantuan.core gargantuan.runtime)

if(APPLE)
	target_link_options(gargantuan.editor PRIVATE "LINKER:-no_warn_duplicate_libraries")
endif()
