
# Custom CMake module for finding "PicoRenderer" files
# Written by Lukas Hermanns on 24/08/2015

# Macros

macro(_PR_APPEND_LIBRARIES _list _release)
	set(_debug ${_release}_DEBUG)
	if(${_debug})
		set(${_list} ${${_list}} optimized ${${_release}} debug ${${_debug}})
	else()
		set(${_list} ${${_list}} ${${_release}})
	endif()
endmacro()

# Find library

find_path(PicoRenderer_INCLUDE_DIR pico.h)

find_library(PicoRenderer_LIBRARY NAMES pico_renderer)
find_library(PicoRenderer_LIBRARY_DEBUG NAMES pico_renderer)

# Setup package handle

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	PicoRenderer
	DEFAULT_MSG
	PicoRenderer_INCLUDE_DIR
    PicoRenderer_LIBRARY
    PicoRenderer_LIBRARY_DEBUG
)

if(PICORENDERER_FOUND)
	set(PicoRenderer_FOUND TRUE)
	_PR_APPEND_LIBRARIES(PicoRenderer_LIBRARIES PicoRenderer_LIBRARY)
endif(PICORENDERER_FOUND)
