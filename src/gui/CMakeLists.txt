add_library(sorcery_gui)

target_include_directories(sorcery_gui PUBLIC
	${CMAKE_SOURCE_DIR}/inc
	${CMAKE_SOURCE_DIR}/ext
)

target_link_libraries(sorcery_gui PUBLIC
	dear_imgui
	Freetype::Freetype
	${SDL2_LIBRARIES}
)

target_sources(sorcery_gui PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/modal.cpp
	${CMAKE_CURRENT_LIST_DIR}/dialog.cpp
	${CMAKE_CURRENT_LIST_DIR}/frame.cpp
	${CMAKE_CURRENT_LIST_DIR}/input.cpp
	${CMAKE_CURRENT_LIST_DIR}/menu.cpp
	${CMAKE_CURRENT_LIST_DIR}/menuaction.cpp
	${CMAKE_CURRENT_LIST_DIR}/menubuilder.cpp
	${CMAKE_CURRENT_LIST_DIR}/message.cpp
	${CMAKE_CURRENT_LIST_DIR}/popup.cpp
	${CMAKE_CURRENT_LIST_DIR}/videoplayer.cpp
)