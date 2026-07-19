include_guard(GLOBAL)

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