# Apple Clang does not yet implement C++26 reflection. Keep the upstream
# GCC path available and use a C++23 enum adapter for the macOS build.
option(SORCERY_PORTABLE_ENUMS "Use magic_enum instead of C++26 reflection" ${APPLE})

if(SORCERY_PORTABLE_ENUMS)
    set(CMAKE_CXX_STANDARD 23)
else()
    set(CMAKE_CXX_STANDARD 26)
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
