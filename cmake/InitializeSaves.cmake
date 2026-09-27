# Seed a fresh runtime directory without replacing the player's existing saves.
if(NOT DEFINED SORCERY_SOURCE_SAVES OR NOT DEFINED SORCERY_DEST_SAVES)
    message(FATAL_ERROR "Source and destination save directories are required")
endif()
file(MAKE_DIRECTORY "${SORCERY_DEST_SAVES}/characters" "${SORCERY_DEST_SAVES}/states")
file(GLOB_RECURSE seed_files LIST_DIRECTORIES FALSE
    RELATIVE "${SORCERY_SOURCE_SAVES}" "${SORCERY_SOURCE_SAVES}/*")
foreach(seed IN LISTS seed_files)
    if(NOT EXISTS "${SORCERY_DEST_SAVES}/${seed}")
        get_filename_component(parent "${SORCERY_DEST_SAVES}/${seed}" DIRECTORY)
        file(MAKE_DIRECTORY "${parent}")
        configure_file("${SORCERY_SOURCE_SAVES}/${seed}" "${SORCERY_DEST_SAVES}/${seed}" COPYONLY)
    endif()
endforeach()
