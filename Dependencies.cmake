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