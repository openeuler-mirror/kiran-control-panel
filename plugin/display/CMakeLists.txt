cmake_minimum_required(VERSION 3.0)

project(kiran-cpanel-display VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets DBus X11Extras Svg LinguistTools)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRANWIDGETS_QT5 REQUIRED kiranwidgets-qt5)

pkg_search_module(KIRAN_CONTROL_PANEL REQUIRED kiran-control-panel)
pkg_get_variable(CPANEL_PLUGIN_DIR kiran-control-panel plugin_location)
pkg_get_variable(CPANEL_DESKTOP_DIR kiran-control-panel plugin_desktop_location)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h" "src/*.ui")
file(GLOB_RECURSE QRC "resources/*.qrc")

set(TS_FILES "translations/kiran-cpanel-display.zh_CN.ts")
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

include_directories(${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/src)

add_library(${PROJECT_NAME} SHARED ${SRC} ${QRC} ${QM_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        src/
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRANWIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CONTROL_PANEL_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        Qt5::DBus
        Qt5::X11Extras
        Qt5::Svg)

#安装插件到指定目录
install(TARGETS ${PROJECT_NAME} DESTINATION ${CPANEL_PLUGIN_DIR}/)

#安装插件desktop
configure_file(data/kiran-cpanel-display.desktop.in ${CMAKE_BINARY_DIR}/kiran-cpanel-display.desktop @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/kiran-cpanel-display.desktop DESTINATION ${CPANEL_DESKTOP_DIR}/ )

SET(link_source ${CPANEL_DESKTOP_DIR}/${PROJECT_NAME}.desktop)
SET(link_target ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-link.desktop)
add_custom_target(kcp-applications-desktop ALL COMMAND ln -sf ${link_source} ${link_target})
install(FILES ${link_target}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/applications/
        RENAME ${PROJECT_NAME}.desktop)

#安装插件图标
set(KCP_ICON_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-display-24.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/24x24/apps RENAME kcp-display.png)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-display-32.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/32x32/apps RENAME kcp-display.png)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-display-48.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/48x48/apps RENAME kcp-display.png)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-display-64.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/64x64/apps RENAME kcp-display.png)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-display-96.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/96x96/apps RENAME kcp-display.png)

#安装翻译文件
set(TRANSLATION_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/translations)
configure_file("data/config.h.in" ${CMAKE_BINARY_DIR}/config.h)
install(FILES ${QM_FILES} DESTINATION ${TRANSLATION_INSTALL_DIR} )