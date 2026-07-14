add_library(sorcery_engine)

target_include_directories(sorcery_engine PUBLIC
	${CMAKE_SOURCE_DIR}/inc
	${CMAKE_SOURCE_DIR}/ext
)

target_link_libraries(sorcery_engine PUBLIC
	dear_imgui
	Freetype::Freetype
	uuid
	${SDL2_LIBRARIES}
)

target_sources(sorcery_engine PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/automap.cpp
	${CMAKE_CURRENT_LIST_DIR}/graveyard.cpp
	${CMAKE_CURRENT_LIST_DIR}/engine.cpp
)