find_program(GLSL_COMPILER glslc REQUIRED)
if(APPLE)
	find_program(SPIRV_CROSS spirv-cross)
	find_program(XCRUN xcrun)

	if(NOT SPIRV_CROSS)
		message(WARNING "Missing spirv-cross, install via `brew install spirv-cross` to compile Metal shaders.")
	endif()
endif()

file(GLOB_RECURSE SHADER_SOURCES CONFIGURE_DEPENDS "*.vert" "*.frag")

set(SHADER_OUTPUTS "")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/shaders")
foreach(SHADER IN LISTS SHADER_SOURCES)
	get_filename_component(SHADER_NAME ${SHADER} NAME)

	set(SPIRV_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.spv")
	add_custom_command(
		OUTPUT ${SPIRV_OUTPUT}
		COMMAND ${GLSL_COMPILER} ${SHADER} -o ${SPIRV_OUTPUT}
		DEPENDS ${SHADER}
		COMMENT "Compiling SPIR-V ${SHADER_NAME}"
	)
	list(APPEND SHADER_OUTPUTS ${SPIRV_OUTPUT})

	if(APPLE AND SPIRV_CROSS AND XCRUN)
		set(METAL_SRC "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.metal")
		set(METAL_AIR "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.air")
		set(METALLIB_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.metallib")

		# SPIR-V -> MSL
		add_custom_command(
			OUTPUT ${METAL_SRC}
			COMMAND ${SPIRV_CROSS} ${SPIRV_OUTPUT} --msl --output ${METAL_SRC}
			DEPENDS ${SPIRV_OUTPUT}
			COMMENT "Cross-compiling SPIR-V to MSL: ${SHADER_NAME}"
		)

		# MSL -> AIR (.air) -> Metal Library
		add_custom_command(
			OUTPUT ${METALLIB_OUTPUT}
			COMMAND ${XCRUN} -sdk macosx metal -c ${METAL_SRC} -o ${METAL_AIR}
			COMMAND ${XCRUN} -sdk macosx metallib ${METAL_AIR} -o ${METALLIB_OUTPUT}
			DEPENDS ${METAL_SRC}
			COMMENT "Building Metal Library: ${SHADER_NAME}.metallib"
		)

		list(APPEND SHADER_OUTPUTS ${METALLIB_OUTPUT})
	endif()
endforeach()
