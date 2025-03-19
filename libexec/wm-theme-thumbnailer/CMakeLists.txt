set(TARGET_NAME wm-theme-thumbnailer)

find_package(PkgConfig REQUIRED)
pkg_search_module(GTK REQUIRED gtk+-3.0)
pkg_search_module(MARCO_PRIVATE REQUIERD libmarco-private)

add_executable(${TARGET_NAME} ./marco-main.c)

target_include_directories(${TARGET_NAME} PRIVATE
    ${PROJECT_BINARY_DIR}
    ${GTK_INCLUDE_DIRS}
    ${MARCO_PRIVATE_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
    ${GTK_LIBRARIES}
    ${MARCO_PRIVATE_LIBRARIES})

install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_FULL_LIBEXECDIR})
