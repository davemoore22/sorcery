add_executable(sorcery_enum_tests enum_roundtrip.cpp ../src/types/meta.cpp)
target_include_directories(sorcery_enum_tests PRIVATE ../inc)
if(SORCERY_PORTABLE_ENUMS)
    target_compile_definitions(sorcery_enum_tests PRIVATE SORCERY_PORTABLE_ENUMS)
    target_link_libraries(sorcery_enum_tests PRIVATE magic_enum::magic_enum)
else()
    target_compile_options(sorcery_enum_tests PRIVATE -freflection)
endif()
add_test(NAME enum_resource_compatibility COMMAND sorcery_enum_tests)
add_test(NAME preserve_saves_on_rebuild
    COMMAND ${CMAKE_COMMAND}
        "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/save-initialization"
        "-DPROJECT_SOURCE=${CMAKE_SOURCE_DIR}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/initialize_saves.cmake")
