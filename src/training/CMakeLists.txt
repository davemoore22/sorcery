add_library(sorcery_training)

target_include_directories(sorcery_training PUBLIC
	${CMAKE_SOURCE_DIR}/inc
)

target_link_libraries(sorcery_training PUBLIC
	dear_imgui
	${SDL2_LIBRARIES}
)

target_sources(sorcery_training PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/create.cpp
	${CMAKE_CURRENT_LIST_DIR}/edit.cpp
	${CMAKE_CURRENT_LIST_DIR}/rite.cpp
	${CMAKE_CURRENT_LIST_DIR}/rename.cpp
	${CMAKE_CURRENT_LIST_DIR}/reclass.cpp
	${CMAKE_CURRENT_LIST_DIR}/roster.cpp
	${CMAKE_CURRENT_LIST_DIR}/select.cpp
	${CMAKE_CURRENT_LIST_DIR}/training.cpp
)