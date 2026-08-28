add_library(sorcery_display)

target_include_directories(sorcery_display PUBLIC
	${CMAKE_SOURCE_DIR}/inc
)

target_link_libraries(sorcery_display PUBLIC
	dear_imgui
	Freetype::Freetype
	uuid
	SimpleIni::SimpleIni
	${SDL2_LIBRARIES}
)

target_sources(sorcery_display PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/animation.cpp
	${CMAKE_CURRENT_LIST_DIR}/display.cpp
	${CMAKE_CURRENT_LIST_DIR}/framebuffer.cpp
	${CMAKE_CURRENT_LIST_DIR}/render.cpp
	${CMAKE_CURRENT_LIST_DIR}/ui/ui.cpp
	${CMAKE_CURRENT_LIST_DIR}/ui/popupstore.cpp
	${CMAKE_CURRENT_LIST_DIR}/ui/screenrenderer.cpp
)