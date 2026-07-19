add_library(sorcery_types)

target_include_directories(sorcery_types PUBLIC
	${CMAKE_SOURCE_DIR}/inc
	${CMAKE_SOURCE_DIR}/ext
)

target_link_libraries(sorcery_types PUBLIC
	dear_imgui
	jsoncpp
	dl
	SimpleIni::SimpleIni
	${SDL2_LIBRARIES}
)

target_sources(sorcery_types PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/character.cpp
	${CMAKE_CURRENT_LIST_DIR}/component.cpp
	${CMAKE_CURRENT_LIST_DIR}/config.cpp
	${CMAKE_CURRENT_LIST_DIR}/dice.cpp
	${CMAKE_CURRENT_LIST_DIR}/error.cpp
	${CMAKE_CURRENT_LIST_DIR}/explore.cpp
	${CMAKE_CURRENT_LIST_DIR}/game.cpp
	${CMAKE_CURRENT_LIST_DIR}/image.cpp
	${CMAKE_CURRENT_LIST_DIR}/inventory.cpp
	${CMAKE_CURRENT_LIST_DIR}/item.cpp
	${CMAKE_CURRENT_LIST_DIR}/itemtype.cpp
	${CMAKE_CURRENT_LIST_DIR}/level.cpp
	${CMAKE_CURRENT_LIST_DIR}/monster.cpp
	${CMAKE_CURRENT_LIST_DIR}/monstertype.cpp
	${CMAKE_CURRENT_LIST_DIR}/scopedtimer.cpp
	${CMAKE_CURRENT_LIST_DIR}/state.cpp
	${CMAKE_CURRENT_LIST_DIR}/tile.cpp
)
