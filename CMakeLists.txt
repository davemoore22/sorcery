# Project Definition
cmake_minimum_required(VERSION 3.22...3.22)
set(CMAKE_CXX_STANDARD 23)
project(sorcery VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Prerequisites
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Freetype REQUIRED)

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

target_include_directories(dear_imgui PRIVATE
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
	stdc++fs
	stdc++exp
	fmt
	jsoncpp
	dear_imgui
	uuid
	sqlite3
)

# Post Build Directories
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory
	cfg)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory
	dat)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory
	doc)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory
	gfx)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory
	vfx)

# Post Build Copying
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/cfg/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/cfg)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/dat/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/dat)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/doc/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/doc)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/gfx/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/gfx)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/vfx/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/vfx)

# Packaging
set(CPACK_PROJECT_NAME sorcery)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)