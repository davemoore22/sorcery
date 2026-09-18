add_library(sorcery_resources)

target_include_directories(sorcery_resources PUBLIC
	${CMAKE_SOURCE_DIR}/inc
)

target_link_libraries(sorcery_resources PUBLIC
	dear_imgui
	Freetype::Freetype
	stb::stb
	${SDL2_LIBRARIES}
)

target_sources(sorcery_resources PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/componentstore.cpp
	${CMAKE_CURRENT_LIST_DIR}/filestore.cpp
	${CMAKE_CURRENT_LIST_DIR}/fontstore.cpp
	${CMAKE_CURRENT_LIST_DIR}/imagestore.cpp
	${CMAKE_CURRENT_LIST_DIR}/itemstore.cpp
	${CMAKE_CURRENT_LIST_DIR}/levelstore.cpp
	${CMAKE_CURRENT_LIST_DIR}/monsterstore.cpp
	${CMAKE_CURRENT_LIST_DIR}/savestore.cpp
	${CMAKE_CURRENT_LIST_DIR}/spellstore.cpp
	${CMAKE_CURRENT_LIST_DIR}/stringstore.cpp
)