cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE GARGANTUAN_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_library(gargantuan.core STATIC)
target_sources(gargantuan.core PUBLIC ${GARGANTUAN_SOURCES})
target_include_directories(gargantuan.core PUBLIC "include")

add_subdirectory("assets/shaders")
target_include_directories(gargantuan.core SYSTEM PRIVATE
	"vendor/argparse/include"
	"vendor/box3d/include"
	"vendor/glm/glm"
	"vendor/luau/Ast/include"
	"vendor/luau/CodeGen/include"
	"vendor/luau/Common/include"
	"vendor/luau/Compiler/include"
	"vendor/luau/Config/include"
	"vendor/luau/Require/include"
	"vendor/luau/VM/include"
	"vendor/magicenum/include"
	"vendor/sdl/include"
	"vendor/sdl_image/include"
	"vendor/sdl_ttf/include"
)

target_link_libraries(gargantuan.core
	PUBLIC
		argparse::argparse
		SDL3::SDL3
		SDL3_ttf-static
		SDL3_image-static
		box3d
		glm
		nlohmann_json
		gargantuan.runtime
		Luau.Compiler
		Luau.CodeGen
		Luau.Ast
		Luau.VM
		Luau.Require
		Luau.Config
		Tracy::TracyClient
)
