
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

file(GLOB_RECURSE SourcesAll ${PROJECT_SOURCE_DIR}/sources/*.c)
file(GLOB_RECURSE HeadersAll ${PROJECT_SOURCE_DIR}/sources/*.h)


# === Include directories ===

include_directories("${PROJECT_SOURCE_DIR}/sources")


# === Executable ===

add_library(
	pico_renderer
	${SourcesAll}
	${HeadersAll}
)

set_target_properties(pico_renderer PROPERTIES LINKER_LANGUAGE C)


