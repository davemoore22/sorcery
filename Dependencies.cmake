include_guard(GLOBAL)

# ---------------------------------------------------------------------------
# Dear ImGui
# ---------------------------------------------------------------------------

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        6ded5230d043aa32c755e65c910c2af5002fb9f9
    GIT_SHALLOW    FALSE
)

FetchContent_MakeAvailable(imgui)

# ---------------------------------------------------------------------------
# Cereal
# ---------------------------------------------------------------------------

set(BUILD_SANDBOX OFF CACHE BOOL
    "Disable cereal sandbox examples"
    FORCE
)

set(SKIP_PERFORMANCE_COMPARISON ON CACHE BOOL
    "Disable cereal performance sandbox"
    FORCE
)

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
# Messages
# ---------------------------------------------------------------------------

message(STATUS "Dear ImGui source: ${imgui_SOURCE_DIR}")
message(STATUS "Imgui Spinner source: ${imgui_spinner_SOURCE_DIR}")
message(STATUS "Imgui Sugar source: ${imgui_sugar_SOURCE_DIR}")

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