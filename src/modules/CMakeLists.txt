add_library(sorcery_modules)

target_include_directories(sorcery_modules PUBLIC
	${CMAKE_SOURCE_DIR}/inc
	${CMAKE_SOURCE_DIR}/ext
)

target_link_libraries(sorcery_modules PUBLIC
	dear_imgui
	${SDL2_LIBRARIES}
)

target_sources(sorcery_modules PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/add.cpp
	${CMAKE_CURRENT_LIST_DIR}/castle.cpp
	${CMAKE_CURRENT_LIST_DIR}/choose.cpp
	${CMAKE_CURRENT_LIST_DIR}/create.cpp
	${CMAKE_CURRENT_LIST_DIR}/edgeoftown.cpp
	${CMAKE_CURRENT_LIST_DIR}/heal.cpp
	${CMAKE_CURRENT_LIST_DIR}/inn.cpp
	${CMAKE_CURRENT_LIST_DIR}/inspect.cpp
	${CMAKE_CURRENT_LIST_DIR}/levelup.cpp
	${CMAKE_CURRENT_LIST_DIR}/nolevelup.cpp
	${CMAKE_CURRENT_LIST_DIR}/pay.cpp
	${CMAKE_CURRENT_LIST_DIR}/remove.cpp
	${CMAKE_CURRENT_LIST_DIR}/recovery.cpp
	${CMAKE_CURRENT_LIST_DIR}/reorder.cpp
	${CMAKE_CURRENT_LIST_DIR}/restart.cpp
	${CMAKE_CURRENT_LIST_DIR}/roster.cpp
	${CMAKE_CURRENT_LIST_DIR}/shop.cpp
	${CMAKE_CURRENT_LIST_DIR}/stay.cpp
	${CMAKE_CURRENT_LIST_DIR}/tavern.cpp
	${CMAKE_CURRENT_LIST_DIR}/temple.cpp
	${CMAKE_CURRENT_LIST_DIR}/training.cpp
)