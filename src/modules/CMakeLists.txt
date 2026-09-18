add_library(sorcery_modules)

target_include_directories(sorcery_modules PUBLIC
	${CMAKE_SOURCE_DIR}/inc
)

target_link_libraries(sorcery_modules PUBLIC
	dear_imgui
	${SDL2_LIBRARIES}
)

target_sources(sorcery_modules PRIVATE


${CMAKE_CURRENT_LIST_DIR}/inn/inn.cpp
${CMAKE_CURRENT_LIST_DIR}/inn/recovery.cpp
${CMAKE_CURRENT_LIST_DIR}/inn/result.cpp
${CMAKE_CURRENT_LIST_DIR}/inn/stay.cpp
${CMAKE_CURRENT_LIST_DIR}/shop/buy.cpp
${CMAKE_CURRENT_LIST_DIR}/shop/identify.cpp
${CMAKE_CURRENT_LIST_DIR}/shop/sell.cpp
${CMAKE_CURRENT_LIST_DIR}/shop/shop.cpp
${CMAKE_CURRENT_LIST_DIR}/shop/store.cpp
${CMAKE_CURRENT_LIST_DIR}/shop/uncurse.cpp
${CMAKE_CURRENT_LIST_DIR}/tavern/add.cpp
${CMAKE_CURRENT_LIST_DIR}/tavern/inspect.cpp
${CMAKE_CURRENT_LIST_DIR}/tavern/remove.cpp
${CMAKE_CURRENT_LIST_DIR}/tavern/reorder.cpp
${CMAKE_CURRENT_LIST_DIR}/tavern/tavern.cpp
${CMAKE_CURRENT_LIST_DIR}/temple/heal.cpp
${CMAKE_CURRENT_LIST_DIR}/temple/pay.cpp
${CMAKE_CURRENT_LIST_DIR}/temple/temple.cpp
${CMAKE_CURRENT_LIST_DIR}/castle.cpp
${CMAKE_CURRENT_LIST_DIR}/edgeoftown.cpp
${CMAKE_CURRENT_LIST_DIR}/restart.cpp

)