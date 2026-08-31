add_library(sorcery_core)

target_include_directories(sorcery_core PUBLIC
	${CMAKE_SOURCE_DIR}/inc
)

target_link_libraries(sorcery_core PUBLIC
	dear_imgui
	Freetype::Freetype
	uuid
	SimpleIni::SimpleIni
	${SDL2_LIBRARIES}
)

target_sources(sorcery_core PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/application.cpp
	${CMAKE_CURRENT_LIST_DIR}/audioplayer.cpp
	${CMAKE_CURRENT_LIST_DIR}/controller/actionhandler.cpp
	${CMAKE_CURRENT_LIST_DIR}/controller/controller.cpp
	${CMAKE_CURRENT_LIST_DIR}/controller/inputhandler.cpp
	${CMAKE_CURRENT_LIST_DIR}/controller/menuaction.cpp
	${CMAKE_CURRENT_LIST_DIR}/controller/menubuilder.cpp
	${CMAKE_CURRENT_LIST_DIR}/controller/menuhandler.cpp
	${CMAKE_CURRENT_LIST_DIR}/context.cpp
	${CMAKE_CURRENT_LIST_DIR}/module.cpp
	${CMAKE_CURRENT_LIST_DIR}/random.cpp
	${CMAKE_CURRENT_LIST_DIR}/resources.cpp
	${CMAKE_CURRENT_LIST_DIR}/system.cpp
)