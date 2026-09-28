add_library(sorcery_game)

target_include_directories(sorcery_game PUBLIC
	${CMAKE_SOURCE_DIR}/inc
)

target_link_libraries(sorcery_game PUBLIC
	${CMAKE_DL_LIBS}
)

target_include_directories(sorcery_game PUBLIC
	${CMAKE_SOURCE_DIR}/inc
)

target_sources(sorcery_game PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/game.cpp
	${CMAKE_CURRENT_LIST_DIR}/cheat.cpp
	${CMAKE_CURRENT_LIST_DIR}/spellcasting.cpp
)