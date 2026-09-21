add_library(sorcery_magic)

target_include_directories(sorcery_magic PUBLIC
	${CMAKE_SOURCE_DIR}/inc
)

target_link_libraries(sorcery_magic PUBLIC
	${CMAKE_DL_LIBS}
)

target_sources(sorcery_magic PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/castcontext.cpp
)

set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/meta.cpp
    PROPERTIES
        COMPILE_OPTIONS "-freflection"
)