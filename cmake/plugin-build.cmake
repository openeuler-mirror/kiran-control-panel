pkg_search_module(KIRAN_CONTROL_CENTER REQUIRED kiran-control-center)
include_directories(${KIRAN_CONTROL_CENTER_INCLUDE_DIRS})

file(GLOB_RECURSE PLUGIN_BUILD_SOURCES "src/plugin_build/*.cpp")
file(GLOB_RECURSE PLUGIN_BUILD_HEADERS "src/plugin_build/*.h")

#暂时通过cmake命令传入插件位置,不通过解析命令来获取
#检测控制中心二进制文件是否存在(依赖于控制中心提供的插件目录),不存在则报错
#获取插件Desktop文件放置位置，插件Icon文件放置位置
#include(control-center-dir.cmake)
#GET_CONTROL_CENTER_DIR( CONTROL_CENTER_PLUGIN_DESKTOP_DIR CONTROL_CENTER_PLUGIN_ICON_DIR )

pkg_search_module(KIRAN_CONTROL_CENTER REQUIRED kiran-control-center)
include_directories(${KIRAN_CONTROL_CENTER_INCLUDE_DIRS})

add_library(${PROJECT_NAME}
        SHARED
        ${PLUGIN_BUILD_SOURCES}
        ${PLUGIN_BUILD_HEADERS}
        ${SRC}
        ${QM_FILES})

configure_file(${CMAKE_SOURCE_DIR}/config/kiran-timedate-manager-plugin.desktop.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop" DESTINATION ${PLUGIN_DESKTOP_DIR} )
install(TARGETS ${PROJECT_NAME} DESTINATION ${KDTM_LIB_INSTALL_DIR})