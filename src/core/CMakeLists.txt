add_library(sorcery_core)

target_include_directories(sorcery_core PUBLIC
	${CMAKE_SOURCE_DIR}/inc
	${CMAKE_SOURCE_DIR}/ext
)

target_link_libraries(sorcery_core PUBLIC
	dear_imgui
	Freetype::Freetype
	uuid
	sqlite3
	${SDL2_LIBRARIES}
)

target_sources(sorcery_core PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/animation.cpp
	${CMAKE_CURRENT_LIST_DIR}/application.cpp
	${CMAKE_CURRENT_LIST_DIR}/audioplayer.cpp
	${CMAKE_CURRENT_LIST_DIR}/controller.cpp
	${CMAKE_CURRENT_LIST_DIR}/context.cpp
	${CMAKE_CURRENT_LIST_DIR}/database.cpp
	${CMAKE_CURRENT_LIST_DIR}/display.cpp
	${CMAKE_CURRENT_LIST_DIR}/random.cpp
	${CMAKE_CURRENT_LIST_DIR}/render.cpp
	${CMAKE_CURRENT_LIST_DIR}/resources.cpp
	${CMAKE_CURRENT_LIST_DIR}/system.cpp
	${CMAKE_CURRENT_LIST_DIR}/ui.cpp
)