set(GLM_ENABLE_CXX_20 ON CACHE BOOL "" FORCE)
set(LUAU_BUILD_CLI OFF CACHE BOOL "" FORCE)
set(LUAU_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_DISABLE_PCH ON CACHE BOOL "" FORCE)
set(SDL_RENDER ON CACHE BOOL "" FORCE)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(argparse SYSTEM)
add_subdirectory(box3d SYSTEM)
add_subdirectory(glm SYSTEM)
add_subdirectory(json SYSTEM)
add_subdirectory(luau SYSTEM)
add_subdirectory(sdl SYSTEM)
add_subdirectory(sdl_image SYSTEM)
add_subdirectory(sdl_ttf SYSTEM)

################################################################################
# Tracy
################################################################################

option(GARGANTUAN_TRACY "Compile in the Tracy client" OFF)
set(TRACY_ENABLE ${GARGANTUAN_TRACY} CACHE BOOL "" FORCE)
set(TRACY_ON_DEMAND ON CACHE BOOL "" FORCE)
set(TRACY_ONLY_LOCALHOST ON CACHE BOOL "" FORCE)
set(TRACY_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
add_subdirectory(tracy)

option(
	GARGANTUAN_TRACY_TOOLS
	"Build tracy-profiler, tracy-capture and tracy-csvexport"
	OFF
)
if(GARGANTUAN_TRACY_TOOLS)
	include(ExternalProject)

	if(NOT WIN32 AND NOT APPLE AND NOT DEFINED ENV{WAYLAND_DISPLAY})
		set(GARGANTUAN_TRACY_LEGACY ON)
	else()
		set(GARGANTUAN_TRACY_LEGACY OFF)
	endif()

	foreach(GARGANTUAN_TRACY_TOOL capture csvexport profiler)
		ExternalProject_Add(tracy-${GARGANTUAN_TRACY_TOOL}
									SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/tracy/${GARGANTUAN_TRACY_TOOL}"
			BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/tracy/${GARGANTUAN_TRACY_TOOL}"
			CMAKE_ARGS
				-DCMAKE_BUILD_TYPE=Release
				-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
				-DLEGACY=${GARGANTUAN_TRACY_LEGACY}
		)
	endforeach()
endif()
