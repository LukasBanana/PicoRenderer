
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

file(GLOB_RECURSE SourcesAll ${PROJECT_SOURCE_DIR}/src/*.*)

file(GLOB SourcesRasterizer ${PROJECT_SOURCE_DIR}/src/rasterizer/*.*)
file(GLOB SourcesPlatformBase ${PROJECT_SOURCE_DIR}/src/platform/*.*)

if(WIN32)
	file(GLOB SourcesPlatform ${PROJECT_SOURCE_DIR}/src/platform/win32/*.*)
	include_directories("${PROJECT_SOURCE_DIR}/src/platform/win32")
else()
	file(GLOB SourcesPlatform ${PROJECT_SOURCE_DIR}/src/platform/posix/*.*)
	include_directories("${PROJECT_SOURCE_DIR}/src/platform/posix")
endif()


# === Include directories ===

include_directories("${PROJECT_SOURCE_DIR}/src/rasterizer")


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


# === Executable ===

add_library(
	pico_renderer
	${SourcesAll}
)

set_target_properties(pico_renderer PROPERTIES LINKER_LANGUAGE C)


