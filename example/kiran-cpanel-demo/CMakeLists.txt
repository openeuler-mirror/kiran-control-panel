cmake_minimum_required(VERSION 3.0)

project(kiran-cpanel-demo VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets X11Extras Svg Xml LinguistTools)
pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(GSETTINGS_QT REQUIRED gsettings-qt)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRANWIDGETS REQUIRED kiranwidgets-qt5)

#通过kiran control panel的pkgconfig配置文件取出插件Desktop安装位置、插件共享库安装位置
pkg_search_module(KIRAN_CONTROL_PANEL_PKG REQUIRED kiran-control-panel)
pkg_get_variable(CPANEL_PLUGIN_DIR kiran-control-panel plugin_location)
pkg_get_variable(CPANEL_DESKTOP_DIR kiran-control-panel plugin_desktop_location)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h" "src/*.ui")
file(GLOB DEMO_TS_FILES "translations/*.ts")

qt5_create_translation(DEMO_QM_FILES ${CMAKE_SOURCE_DIR} ${DEMO_TS_FILES})
include_directories(${KIRAN_CONTROL_PANEL_PKG_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED
        ${SRC}
        ${DEMO_QM_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${GLIB_INCLUDE_DIRS}
        ${GSETTINGS_QT_INCLUDE_DIRS}
        ${KIRAN_CONTROL_PANEL_PKG_INCLUDE_DIRS}
        ${KIRANWIDGETS_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Gui
        Qt5::Svg
        Qt5::Widgets
        Qt5::X11Extras
        Qt5::Xml
        X11
        ${GLIB_LIBRARIES}
        ${GSETTINGS_QT_LIBRARIES}
        ${KIRANWIDGETS_LIBRARIES}
        ${KLOG_QT5_LIBRARIES})

set(PLUGIN_ICONS_DIR ${INSTALL_DATADIR}/icons/hicolor/scalable/apps )
set(PLUGIN_TRANSLATION_DIR ${INSTALL_DATADIR}/${PROJECT_NAME}/translation/)
configure_file(data/kiran-cpanel-demo-config.h.in ${CMAKE_BINARY_DIR}/plugin-demo-config.h)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CPANEL_PLUGIN_DIR}/)

configure_file(data/kiran-cpanel-demo.desktop.in ${CMAKE_BINARY_DIR}/kiran-cpanel-demo.desktop @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/kiran-cpanel-demo.desktop DESTINATION ${CPANEL_DESKTOP_DIR}/ )

install(FILES ${DEMO_QM_FILES} DESTINATION ${PLUGIN_TRANSLATION_DIR} )

set(INSTALL_ICONS
        "data/kcp-demo-subitem-1.svg"
        "data/kcp-demo-subitem-2.svg"
        "data/kcp-demo.svg")
install(FILES ${INSTALL_ICONS} DESTINATION ${PLUGIN_ICONS_DIR})