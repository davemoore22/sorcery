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

add_executable(sorcery_menu_tests menu_navigation.cpp ../src/display/ui/menunavigation.cpp)
target_include_directories(sorcery_menu_tests PRIVATE ../inc)
target_link_libraries(sorcery_menu_tests PRIVATE dear_imgui)
add_test(NAME menu_keyboard_navigation COMMAND sorcery_menu_tests)

# Exercise real game/resource objects with isolated saves and no graphics window.
add_executable(sorcery_save_tests save_restore.cpp)
target_include_directories(sorcery_save_tests PRIVATE ../inc)
get_target_property(sorcery_runtime_libraries sorcery LINK_LIBRARIES)
target_link_libraries(sorcery_save_tests PRIVATE ${sorcery_runtime_libraries})
# FileStore resolves the runtime assets next to the executable, as in the game.
add_dependencies(sorcery_save_tests sorcery)
add_test(NAME saved_game_runtime_restoration COMMAND sorcery_save_tests)
set_tests_properties(saved_game_runtime_restoration PROPERTIES ENVIRONMENT "SDL_AUDIODRIVER=dummy")
