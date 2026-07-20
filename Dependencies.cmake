include_guard(GLOBAL)

# ---------------------------------------------------------------------------
# Dear ImGui
# ---------------------------------------------------------------------------


MESSAGE("Grabbing Dear ImGui from https://github.com/ocornut/imgui.git")

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        8936b58
    GIT_SHALLOW    FALSE
)

FetchContent_MakeAvailable(imgui)

# ---------------------------------------------------------------------------
# Cereal
# ---------------------------------------------------------------------------

MESSAGE("Grabbing Cereal from https://github.com/USCiLab/cereal.git")

set(SKIP_PORTABILITY_TEST ON CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SANDBOX OFF CACHE BOOL "" FORCE)
set(BUILD_DOC OFF CACHE BOOL "" FORCE)
set(JUST_INSTALL_CEREAL ON CACHE BOOL "" FORCE)

FetchContent_Declare(
	cereal
	GIT_REPOSITORY https://github.com/USCiLab/cereal.git
	GIT_TAG        22a1b369f39be918ca79206a83c4facd759f9105
	GIT_SHALLOW    FALSE
)

# Avoid building cereal's tests/examples.
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(SKIP_PERFORMANCE_COMPARISON ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(cereal)

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG        b233b96e49d371bad00300f59b5ba581100b8745
    GIT_SHALLOW    FALSE
)

FetchContent_MakeAvailable(magic_enum)

# ---------------------------------------------------------------------------
# SimpleIni
# ---------------------------------------------------------------------------

Message("Grabbing SimpleIni from https://github.com/brofield/simpleini.git")

FetchContent_Declare(
    simpleini
    GIT_REPOSITORY https://github.com/brofield/simpleini.git
    GIT_TAG        e260c3217bd37b3efc33767b6b7a49e38c1481e7
    GIT_SHALLOW    FALSE
)

FetchContent_MakeAvailable(simpleini)

# ---------------------------------------------------------------------------
# stb
# ---------------------------------------------------------------------------

MESSAGE("Grabbing stb from https://github.com/nothings/stb.git")

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        31c1ad37456438565541f4919958214b6e762fb4
    GIT_SHALLOW    FALSE

    # stb is a source/header collection, so we supply our own target.
    SOURCE_SUBDIR  cmake/fetchcontent-disabled
)

FetchContent_MakeAvailable(stb)

# stb has no upstream CMake target, so provide one.
add_library(stb INTERFACE)
add_library(stb::stb ALIAS stb)

target_include_directories(stb
    SYSTEM INTERFACE
        "${stb_SOURCE_DIR}"
)

# ---------------------------------------------------------------------------
# Imgui Spinner
# ---------------------------------------------------------------------------

MESSAGE("Grabbing Imgui Spinner from https://github.com/dalerank/imspinner.git")

FetchContent_Declare(
    imgui_spinner
    GIT_REPOSITORY https://github.com/dalerank/imspinner.git
    GIT_TAG        ffe57a9cf741a92bdb6042cd4f8eb152b9c95b1d
    GIT_SHALLOW    FALSE
)

FetchContent_MakeAvailable(imgui_spinner)

add_library(sorcery_imgui_spinner INTERFACE)
add_library(imgui_spinner::imgui_spinner ALIAS sorcery_imgui_spinner)

target_include_directories(sorcery_imgui_spinner
    SYSTEM INTERFACE
        "${imgui_spinner_SOURCE_DIR}"
)

# ---------------------------------------------------------------------------
# Imgui Sugar
# ---------------------------------------------------------------------------

MESSAGE("Grabbing Imgui Sugar from https://github.com/mnesarco/imgui_sugar.git")

FetchContent_Declare(
    imgui_sugar
    GIT_REPOSITORY https://github.com/mnesarco/imgui_sugar.git
    GIT_TAG        1092a7344cc528a533752fbccd69c270ad641e4f
    GIT_SHALLOW    FALSE
)

FetchContent_MakeAvailable(imgui_sugar)

add_library(sorcery_imgui_sugar INTERFACE)
add_library(imgui_sugar::imgui_sugar ALIAS sorcery_imgui_sugar)

target_include_directories(sorcery_imgui_sugar
    SYSTEM INTERFACE
        "${imgui_sugar_SOURCE_DIR}"
)


# ---------------------------------------------------------------------------
# Imgui Toggle
# ---------------------------------------------------------------------------

MESSAGE("Grabbing Imgui Toggle from https://github.com/cmdwtf/imgui_toggle.git")
MESSAGE("Applying patch to Imgui Toggle for Dear ImGui 1.92.8 compatibility")

FetchContent_Declare(
    imgui_toggle
    GIT_REPOSITORY https://github.com/cmdwtf/imgui_toggle.git
    GIT_TAG        bfd17d7e73558b1fb1ecf70fd0a6481c1b05cc69
    GIT_SHALLOW    FALSE

   PATCH_COMMAND
        "${CMAKE_COMMAND}"
        "-DSOURCE_DIR=<SOURCE_DIR>"
        "-DPATCH_FILE=${CMAKE_SOURCE_DIR}/imgui_toggle-imgui-1.92.8.patch"
        -P
        "${CMAKE_SOURCE_DIR}/ApplyPatch.cmake"
)

FetchContent_MakeAvailable(imgui_toggle)

# ---------------------------------------------------------------------------
# Messages
# ---------------------------------------------------------------------------

message(STATUS "Dear ImGui source: ${imgui_SOURCE_DIR}")
message(STATUS "Imgui Spinner source: ${imgui_spinner_SOURCE_DIR}")
message(STATUS "Imgui Sugar source: ${imgui_sugar_SOURCE_DIR}")
message(STATUS "Imgui Toggle source: ${imgui_toggle_SOURCE_DIR}")

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