
# === CMake lists for the PicoRenderer - (07/09/2014) ===

cmake_minimum_required(VERSION 2.8)
project(PicoRenderer)


# === Build path ===

set(dir ${CMAKE_CURRENT_BINARY_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)


# === Preprocessor definitions ===

add_definitions(-D_CRT_SECURE_NO_WARNINGS)


# === Global files ===

file(GLOB HeadersMain ${PROJECT_SOURCE_DIR}/inc/*.*)
file(GLOB SourcesMain ${PROJECT_SOURCE_DIR}/src/*.*)

file(GLOB SourcesRasterizer ${PROJECT_SOURCE_DIR}/src/rasterizer/*.*)
file(GLOB SourcesPlatformBase ${PROJECT_SOURCE_DIR}/src/platform/*.*)

if(WIN32)
	file(GLOB SourcesPlatform ${PROJECT_SOURCE_DIR}/src/platform/win32/*.*)
	file(GLOB SourcesTest ${PROJECT_SOURCE_DIR}/test/win32/*.*)
	include_directories("${PROJECT_SOURCE_DIR}/src/platform/win32")
elseif(APPLE)
	file(GLOB SourcesPlatform ${PROJECT_SOURCE_DIR}/src/platform/macos/*.*)
	file(GLOB SourcesTest ${PROJECT_SOURCE_DIR}/test/macos/*.*)
	include_directories("${PROJECT_SOURCE_DIR}/src/platform/macos")
elseif(UNIX)
	file(GLOB SourcesPlatform ${PROJECT_SOURCE_DIR}/src/platform/linux/*.*)
	file(GLOB SourcesTest ${PROJECT_SOURCE_DIR}/test/linux/*.*)
	include_directories("${PROJECT_SOURCE_DIR}/src/platform/linux")
else()
	message(FATAL_ERROR "Unsupported platform")
endif()


# === Include directories ===

include_directories("${PROJECT_SOURCE_DIR}/src/rasterizer")
include_directories("${PROJECT_SOURCE_DIR}/src/")
include_directories("${PROJECT_SOURCE_DIR}/inc/")


# === Source groups ===

source_group(
	"src\\rasterizer"
	FILES
	${SourcesRasterizer}
)

source_group(
	"src\\platform"
	FILES
	${SourcesPlatformBase}
	${SourcesPlatform}
)

source_group(
	"inc"
	FILES
	${HeadersMain}
)

source_group(
	"src"
	FILES
	${SourcesMain}
)

source_group(
	"test1"
	FILES
	${SourcesTest}
)


# === Executable ===

add_library(
	pico_renderer
	${SourcesMain}
	${HeadersMain}
	${SourcesRasterizer}
	${SourcesPlatformBase}
	${SourcesPlatform}
	${PROJECT_SOURCE_DIR}/src/pico.c
)

if(APPLE)
	add_executable(
		test1
		MACOSX_BUNDLE
		${SourcesTest}
	)
else()
	add_executable(
		test1
		${SourcesTest}
	)
endif()


if(WIN32)
	target_link_libraries(test1 pico_renderer)
elseif(APPLE)
	find_library(COCOA_LIBRARY Cocoa)
	target_link_libraries(test1 ${COCOA_LIBRARY} pico_renderer)
elseif(UNIX)
	target_link_libraries(pico_renderer X11)
	target_link_libraries(test1 pico_renderer X11)
endif()

set_target_properties(pico_renderer PROPERTIES LINKER_LANGUAGE C)
set_target_properties(test1 PROPERTIES LINKER_LANGUAGE C)




