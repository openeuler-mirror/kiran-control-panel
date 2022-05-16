cmake_minimum_required(VERSION 3.5)

project(kiran-cpanel-timedate)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus Network LinguistTools)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)

#通过kiran control panel的pkgconfig配置文件取出插件Desktop安装位置、插件共享库安装位置
pkg_search_module(KIRAN_CONTROL_PANEL_PKG REQUIRED kiran-control-panel)
pkg_get_variable(CPANEL_PLUGIN_DIR kiran-control-panel plugin_location)
pkg_get_variable(CPANEL_DESKTOP_DIR kiran-control-panel plugin_desktop_location)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SRC
        "src/*.cpp"
        "src/*.h")

file(GLOB_RECURSE QRC
        "resources/*.qrc")

include_directories("${CMAKE_BINARY_DIR}" "src" )
set( TS_FILES "translations/kiran-cpanel-timedate.zh_CN.ts" )
qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )

add_library(${PROJECT_NAME} SHARED
        ${SRC}
        ${QRC}
        ${QM_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        include
        src/dbus-api-wrapper
        src/timezone
        src/widgets
        src/tools
        src/pages
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_CONTROL_PANEL_PKG_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::Network
        X11
        Xrandr
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES})

#安装插件Desktop
configure_file(config/kiran-cpanel-timedate.desktop.in ${CMAKE_BINARY_DIR}/kiran-cpanel-timedate.desktop @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/kiran-cpanel-timedate.desktop DESTINATION ${CPANEL_DESKTOP_DIR}/ )

SET(link_source ${CPANEL_DESKTOP_DIR}/${PROJECT_NAME}.desktop)
SET(link_target ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-link.desktop)
add_custom_target(kcp-applications-desktop ALL COMMAND ln -sf ${link_source} ${link_target})
install(FILES ${link_target}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/applications/
        RENAME ${PROJECT_NAME}.desktop)

#安装插件icon
set(KCP_ICON_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-timedate-24.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/24x24/apps RENAME kcp-timedate.png)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-timedate-32.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/32x32/apps RENAME kcp-timedate.png)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-timedate-48.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/48x48/apps RENAME kcp-timedate.png)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-timedate-64.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/64x64/apps RENAME kcp-timedate.png)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-timedate-96.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/96x96/apps RENAME kcp-timedate.png)

#安装插件
install(TARGETS ${PROJECT_NAME} DESTINATION ${CPANEL_PLUGIN_DIR})

#安装翻译文件
set(TRANSLATE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/translations/")
configure_file(./config/config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)
install(FILES ${QM_FILES} DESTINATION ${TRANSLATE_INSTALL_DIR} )
