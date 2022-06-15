cmake_minimum_required(VERSION 3.0)

project(kiran-control-panel VERSION 1.0)

set(KIRAN_CONTROL_PANEL_VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets X11Extras Svg LinguistTools)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KIRAN_STYLE_HELPER REQUIRED kiran-style-helper)
#pkg_search_module(GLIB_2 REQUIRED glib-2.0)

include(kcp_variables.cmake)
include(kcp-qdbus-wrapper.cmake)

configure_file(${CMAKE_SOURCE_DIR}/data/config.h.in ${CMAKE_BINARY_DIR}/config.h)
configure_file(${CMAKE_SOURCE_DIR}/data/${PROJECT_NAME}.desktop.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop)
configure_file(${CMAKE_SOURCE_DIR}/data/${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)

file(GLOB_RECURSE include "./include/*")
file(GLOB CONTROL_PANEL_SRC "./src/*")
file(GLOB TS_FILES "../translations/*.ts")

set(RESOURCE ./resources/control-panel-resources.qrc) #资源文件名不能与插件资源名相同，否则会导致插件中部分图片无法正常加载显示。
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

add_executable(${PROJECT_NAME}
        ${common}
        ${include}
        ${CONTROL_PANEL_SRC}
        ${RESOURCE}
        ${QM_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        include
        src
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
        common-widgets
        plugin-info
        Qt5::Core
        Qt5::Gui
        Qt5::Svg
        Qt5::X11Extras
        Qt5::Widgets
        dl
        pthread
        ${KLOG_QT5_LIBRARIES}
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES})

#安装kiran-control-panel
install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_BINDIR}/)
#安装kiran-control-panel插件开发头文件
install(FILES ${include} DESTINATION ${KCP_INSTALL_INCLUDE}/)
#安装kiran-control-panel.desktop
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop" DESTINATION "${INSTALL_DATADIR}/applications/")
#安装kiran-control-panel.pc文件
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION "${INSTALL_LIBDIR}/pkgconfig/")
#安装kiran-control-panel翻译文件
install(FILES ${QM_FILES} DESTINATION ${TRANSLATION_INSTALL_DIR})
#安装kiran-control-panel category desktop文件
file(GLOB CATEGORY_DESKTOP "./data/category/desktop/*.desktop")
install(FILES ${CATEGORY_DESKTOP} DESTINATION ${CATEGORY_DESKTOP_INSTALL_DIR})
#安装kiran-control-panel category icon文件
file(GLOB CATEGORY_ICONS "./data/category/images/*")
install(FILES ${CATEGORY_ICONS} DESTINATION ${CATEGORY_ICON_INSTALL_DIR})
#安装kiran-control-panel icon文件
install(FILES "${CMAKE_SOURCE_DIR}/data/kiran-control-panel.svg" DESTINATION ${INSTALL_DATADIR}/icons/hicolor/)

add_subdirectory(lib)
add_subdirectory(launcher)
add_subdirectory(plugins)
add_subdirectory(data)