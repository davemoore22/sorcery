# Project Definition
cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_PARALLEL_LEVEL 3)
project(sorcery VERSION 0.1.0)

# Prerequisites
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(SFML 2.5.1 COMPONENTS system graphics audio network REQUIRED)
find_package(X11 REQUIRED)
find_package(TGUI 0.9 REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(fmt REQUIRED)

# Source Files
add_executable(${PROJECT_NAME}
	src/allocatepanel.cpp
	src/animation.cpp
	src/application.cpp
	src/attractmode.cpp
	src/attributedisplay.cpp
	src/automap.cpp
	src/banner.cpp
	src/castle.cpp
	src/changeclass.cpp
	src/changename.cpp
	src/character.cpp
	src/characterpanel.cpp
	src/charactersummary.cpp
	src/compass.cpp
	src/compendium.cpp
	src/component.cpp
	src/config.cpp
	src/confirm.cpp
	src/console.cpp
	src/controloverlay.cpp
	src/create.cpp
	src/database.cpp
	src/dialog.cpp
	src/display.cpp
	src/edgeoftown.cpp
	src/edit.cpp
	src/engine.cpp
	src/error.cpp
	src/file.cpp
	src/font.cpp
	src/frame.cpp
	src/game.cpp
	src/graphics.cpp
	src/iconpanel.cpp
	src/iconstore.cpp
	src/infopanel.cpp
	src/inn.cpp
	src/input.cpp
	src/inspect.cpp
	src/keyboard.cpp
	src/layout.cpp
	src/legate.cpp
	src/level.cpp
	src/levelstore.cpp
	src/license.cpp
	src/main.cpp
	src/mainmenu.cpp
	src/manage.cpp
	src/message.cpp
	src/menu.cpp
	src/options.cpp
	src/platform.cpp
	src/random.cpp
	src/render.cpp
	src/reorder.cpp
	src/resourcemanager.cpp
	src/roster.cpp
	src/shop.cpp
	src/spellpanel.cpp
	src/splash.cpp
	src/state.cpp
	src/statusbar.cpp
	src/string.cpp
	src/system.cpp
	src/tavern.cpp
	src/temple.cpp
	src/text.cpp
	src/textfile.cpp
	src/texturestore.cpp
	src/tile.cpp
	src/training.cpp
	src/view.cpp
	src/window.cpp)

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_SOURCE_DIR}/inc
	${CMAKE_SOURCE_DIR}/inc/simpleini
	${CMAKE_SOURCE_DIR}/inc/magicenum
	${CMAKE_SOURCE_DIR}/inc/sqlitemoderncpp
	${CMAKE_SOURCE_DIR}/inc/backwardcpp
	${CMAKE_SOURCE_DIR}/inc/cereal
	${CMAKE_SOURCE_DIR}/inc/sfemovie
	/usr/local/include
	/usr/include/X11)

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
	-Winvalid-pch
	-Wmissing-declarations
	-Wmissing-format-attribute
	-Wmissing-include-dirs
	-Wredundant-decls
	-Wswitch-default
	-Wodr
	-Wfatal-errors
	-fdiagnostics-color=always
	-pthread
	-pg
	-g)

# Link Libraries (Linux Hacks)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE -lX11)

# Standard Link Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
	sfml-audio
	sfml-graphics
	sfml-network
	sfml-system
	tgui
	sqlite3
	sfeMovie
	jsoncpp
	thor
	fmt
	uuid
	bfd
	dl
	unwind
	stdc++fs
	)

# Post Build Directories
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory
	cfg)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory
	dat)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory
	doc)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory
	gui)
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
	${CMAKE_SOURCE_DIR}/gui/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/gui)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/vfx/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/vfx)

# Packaging
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)