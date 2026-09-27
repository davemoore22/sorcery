add_library(sorcery_types)

target_include_directories(sorcery_types PUBLIC
	${CMAKE_SOURCE_DIR}/inc
)

target_link_libraries(sorcery_types PUBLIC
	dear_imgui
	JsonCpp::JsonCpp
	${CMAKE_DL_LIBS}
	SimpleIni::SimpleIni
	${SDL2_LIBRARIES}
)

target_sources(sorcery_types PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/character/character.cpp
	${CMAKE_CURRENT_LIST_DIR}/character/create.cpp
	${CMAKE_CURRENT_LIST_DIR}/character/inventory.cpp
	${CMAKE_CURRENT_LIST_DIR}/character/magic.cpp
	${CMAKE_CURRENT_LIST_DIR}/cheat.cpp
	${CMAKE_CURRENT_LIST_DIR}/component.cpp
	${CMAKE_CURRENT_LIST_DIR}/config.cpp
	${CMAKE_CURRENT_LIST_DIR}/dice.cpp
	${CMAKE_CURRENT_LIST_DIR}/error.cpp
	${CMAKE_CURRENT_LIST_DIR}/game.cpp
	${CMAKE_CURRENT_LIST_DIR}/image.cpp
	${CMAKE_CURRENT_LIST_DIR}/item/item.cpp
	${CMAKE_CURRENT_LIST_DIR}/item/itemtype.cpp
	${CMAKE_CURRENT_LIST_DIR}/meta.cpp
	${CMAKE_CURRENT_LIST_DIR}/monster.cpp
	${CMAKE_CURRENT_LIST_DIR}/monstertype.cpp
	${CMAKE_CURRENT_LIST_DIR}/scopedtimer.cpp
	${CMAKE_CURRENT_LIST_DIR}/state.cpp
	${CMAKE_CURRENT_LIST_DIR}/world/explore.cpp
	${CMAKE_CURRENT_LIST_DIR}/world/level.cpp
	${CMAKE_CURRENT_LIST_DIR}/world/tile.cpp
)

if(SORCERY_PORTABLE_ENUMS)
    target_link_libraries(sorcery_types PRIVATE magic_enum::magic_enum)
    target_compile_definitions(sorcery_types PRIVATE SORCERY_PORTABLE_ENUMS)
else()
    set_source_files_properties(
        ${CMAKE_CURRENT_LIST_DIR}/meta.cpp
        PROPERTIES COMPILE_OPTIONS "-freflection"
    )
endif()
