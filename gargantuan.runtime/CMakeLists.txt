cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE GARGANTUAN_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_library(gargantuan.runtime STATIC)
target_sources(gargantuan.runtime PUBLIC ${GARGANTUAN_SOURCES})
target_include_directories(gargantuan.runtime PUBLIC "include")

target_link_libraries(gargantuan.runtime PUBLIC Luau.VM)
