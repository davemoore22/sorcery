# Project Definition
cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 23)
project(sorcery VERSION 0.1.0)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR
		"In-source builds are not supported. "
		"Use: cmake -S . -B build"
	)
endif()

include(FetchContent)
include(Dependencies.cmake)

if(TARGET cereal::cereal)
	message(STATUS "Found cereal::cereal")
else()
	message(FATAL_ERROR "cereal::cereal target was not created")
endif()

if(TARGET magic_enum::magic_enum)
	message(STATUS "Found magic_enum::magic_enum")
else()
	message(FATAL_ERROR "magic_enum::magic_enum target was not created")
endif()

if(TARGET SimpleIni::SimpleIni)
	message(STATUS "Found SimpleIni::SimpleIni")
else()
	message(FATAL_ERROR "SimpleIni::SimpleIni target was not created")
endif()


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Prerequisites
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Freetype REQUIRED)
find_package(glm REQUIRED)

# Zep
add_definitions(-DZEP_USE_SDL)
add_definitions(-DZEP_FEATURE_CPP_FILE_SYSTEM)
add_definitions(-DZEP_SINGLE_HEADER=1)

# ffmpeg
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
	libavdevice
	libavfilter
	libavformat
	libavcodec
	libswresample
	libswscale
	libavutil
)

# Imgui Library
add_library(dear_imgui EXCLUDE_FROM_ALL)

target_sources(dear_imgui PRIVATE
	ext/imgui/imgui.cpp
	ext/imgui/imgui_demo.cpp
	ext/imgui/imgui_draw.cpp
	ext/imgui/imgui_tables.cpp
	ext/imgui/imgui_widgets.cpp
	ext/imgui/backends/imgui_impl_sdl2.cpp
	ext/imgui/backends/imgui_impl_opengl3.cpp
	ext/imgui/misc/cpp/imgui_stdlib.cpp
	ext/imgui_toggle/imgui_toggle.cpp
	ext/imgui_toggle/imgui_toggle_palette.cpp
	ext/imgui_toggle/imgui_toggle_presets.cpp
	ext/imgui_toggle/imgui_toggle_renderer.cpp
	ext/imgui/misc/freetype/imgui_freetype.cpp
)

target_include_directories(dear_imgui PUBLIC
	${SDL2_INCLUDE_DIRS}
	${SDL2_IMAGE_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	${GLUT_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/ext/imgui
	${CMAKE_SOURCE_DIR}/ext/imgui/backends
	${CMAKE_SOURCE_DIR}/ext/imgui_toggle
	${CMAKE_SOURCE_DIR}/ext/imgui_sugar
	${CMAKE_SOURCE_DIR}/ext/imgui/misc/freetype
)

target_link_libraries(dear_imgui PRIVATE
	Freetype::Freetype
)

target_compile_options(dear_imgui PRIVATE
	-Wno-switch-default
	-Wno-suggest-attribute=format
)

target_compile_definitions(dear_imgui PRIVATE
	IMGUI_ENABLE_FREETYPE
)

# Global Ifdefs
add_library(sorcery_options INTERFACE)
target_compile_definitions(sorcery_options INTERFACE SORCERY_PROFILE)
target_compile_definitions(sorcery_options INTERFACE SORCERY_DEBUG)

target_link_libraries(sorcery_options INTERFACE
	cereal::cereal
	magic_enum::magic_enum
	stb::stb
	SimpleIni::SimpleIni
)

# Global Compile Options
add_library(sorcery_warnings INTERFACE)
target_compile_options(sorcery_warnings INTERFACE
	-Wall
	-Wextra
	-Wpedantic
	-Wno-narrowing
	-Wno-unused-parameter
)

# Main Source Files
include(src/core/CMakeLists.txt)
include(src/engine/CMakeLists.txt)
include(src/frontend/CMakeLists.txt)
include(src/gui/CMakeLists.txt)
include(src/modules/CMakeLists.txt)
include(src/resources/CMakeLists.txt)
include(src/types/CMakeLists.txt)
add_executable(${PROJECT_NAME}
	src/sorcery.cpp
	${sorcery_SOURCES})

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE
	${SDL2_INCLUDE_DIRS}
	${SDL2_IMAGE_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	${GLUT_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/ext
	${CMAKE_SOURCE_DIR}/ext/imgui
	${CMAKE_SOURCE_DIR}/inc)

# Compiler Options
target_compile_options(${PROJECT_NAME} PRIVATE
	-Wall
	-Wextra
	-Wpedantic
	-Wunreachable-code
	-Wunused
	-Wignored-qualifiers
	-Wcast-align
	-Wformat-nonliteral
	-Wformat=2
	-Wsuggest-attribute=format
	-Winvalid-pch
	-Wmissing-declarations
	-Wmissing-format-attribute
	-Wmissing-include-dirs
	-Wredundant-decls
	-Wodr
	-Wfatal-errors
	-fdiagnostics-color=always
	-Wno-narrowing
	-pthread
	-pg
	-g)

set_target_properties(${PROJECT_NAME} PROPERTIES
	BUILD_RPATH "$ORIGIN/lib"
)

target_link_libraries(sorcery_types PRIVATE
	sorcery_warnings
	sorcery_options)
target_link_libraries(sorcery_engine PRIVATE
	sorcery_warnings
	sorcery_options)
target_link_libraries(sorcery_core PRIVATE
	sorcery_warnings
	sorcery_options)
target_link_libraries(sorcery_frontend PRIVATE
	sorcery_warnings
	sorcery_options)
target_link_libraries(sorcery_gui PRIVATE
	sorcery_warnings
	sorcery_options)
target_link_libraries(sorcery_modules PRIVATE
	sorcery_warnings
	sorcery_options)
target_link_libraries(sorcery_resources PRIVATE
	sorcery_warnings
	sorcery_options)

# Link Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
	${SDL2_LIBRARIES}
	${OPENGL_LIBRARIES}
	Freetype::Freetype
	PkgConfig::LIBAV
	Threads::Threads
	OpenGL::GL
	GLEW
	dl
	backtrace
	stdc++fs
	stdc++exp
	jsoncpp
	dear_imgui
	uuid
	sorcery_core
	sorcery_engine
	sorcery_frontend
	sorcery_gui
	sorcery_modules
	sorcery_resources
	sorcery_types
	sorcery_warnings
	cereal::cereal
	magic_enum::magic_enum
	stb::stb
	SimpleIni::SimpleIni
)

MESSAGE("Creating and Populating Post-Build Directories")

# Post Build Copying
set(SORCERY_DIST_DIR "${CMAKE_BINARY_DIR}/dist")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory
		"${SORCERY_DIST_DIR}"
		"${SORCERY_DIST_DIR}/lib"

	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"$<TARGET_FILE:${PROJECT_NAME}>"
		"${SORCERY_DIST_DIR}/"

	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_BINARY_DIR}/lib"
		"${SORCERY_DIST_DIR}/lib"

	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/cfg"
		"${SORCERY_DIST_DIR}/cfg"

	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/dat"
		"${SORCERY_DIST_DIR}/dat"

	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/doc"
		"${SORCERY_DIST_DIR}/doc"

	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/gfx"
		"${SORCERY_DIST_DIR}/gfx"

	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/sav"
		"${SORCERY_DIST_DIR}/sav"

	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/sav/characters"
		"${SORCERY_DIST_DIR}/sav/characters"

	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/sav/characters"
		"${SORCERY_DIST_DIR}/sav/states"

	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/sfx"
		"${SORCERY_DIST_DIR}/sfx"

	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/vfx"
		"${SORCERY_DIST_DIR}/vfx"
)

# Packaging
set(CPACK_PROJECT_NAME sorcery)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)