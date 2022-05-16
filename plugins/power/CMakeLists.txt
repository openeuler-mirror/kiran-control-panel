cmake_minimum_required(VERSION 3.5)

project(kiran-cpanel-power)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#add_definitions(-DTEST)

include(GNUInstallDirs)
set(INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
set(INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(INSTALL_DATADIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR})

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(QGSETTINGS REQUIRED gsettings-qt)
pkg_search_module(UPOWER_GLIB REQUIRED upower-glib)

#通过kiran control panel的pkgconfig配置文件取出插件Desktop安装位置、插件共享库安装位置
pkg_search_module(KIRAN_CONTROL_PANEL_PKG REQUIRED kiran-control-panel)
pkg_get_variable(CPANEL_PLUGIN_DIR kiran-control-panel plugin_location)
pkg_get_variable(CPANEL_DESKTOP_DIR kiran-control-panel plugin_desktop_location)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SRC
        "src/*.cpp"
        "src/*.h"
        "src/*.ui")

file(GLOB_RECURSE QRC
        "resources/*.qrc")

set( TS_FILES "translations/kiran-cpanel-power.zh_CN.ts" )

include_directories(
        "${CMAKE_BINARY_DIR}"
        src
        src/pages
        src/pages/battery-settings
        src/pages/general-settings
        src/pages/power-settings
        src/dbus
        src/common)

qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )

add_library(${PROJECT_NAME} SHARED
        ${SRC}
        ${QRC}
        ${QM_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        src/pages
        src/pages/battery-settings
        src/pages/general-settings
        src/pages/power-settings
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_CONTROL_PANEL_PKG_INCLUDE_DIRS}
        ${QGSETTINGS_INCLUDE_DIRS}
        ${UPOWER_GLIB_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        Qt5::DBus
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KIRAN_CC_DAEMON_LIBRARIES}
        ${KLOG_QT5_LIBRARIES}
        ${QGSETTINGS_LIBRARIES}
        ${UPOWER_GLIB_LIBRARIES})

include(GNUInstallDirs)
# 安装插件翻译
set(KIRAN_POWER_MANAGER_TRANSLATIONS_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/translations)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
install(FILES ${QM_FILES} DESTINATION ${KIRAN_POWER_MANAGER_TRANSLATIONS_DIR})

# 安装插件desktop文件
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/kiran-cpanel-power.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-power.desktop @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/kiran-cpanel-power.desktop DESTINATION ${CPANEL_DESKTOP_DIR}/)

SET(link_source ${CPANEL_DESKTOP_DIR}/${PROJECT_NAME}.desktop)
SET(link_target ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-link.desktop)
add_custom_target(kcp-applications-power-desktop ALL COMMAND ln -sf ${link_source} ${link_target})
install(FILES ${link_target}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/applications/
        RENAME ${PROJECT_NAME}.desktop)

# 安装插件
install(TARGETS ${PROJECT_NAME} DESTINATION ${CPANEL_PLUGIN_DIR}/)

# 安装需要控制面板显示的图标到图标主题目录
set(KCP_ICON_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-power-24.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/24x24/apps RENAME kcp-power.png)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-power-32.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/32x32/apps RENAME kcp-power.png)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-power-48.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/48x48/apps RENAME kcp-power.png)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-power-64.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/64x64/apps RENAME kcp-power.png)
install(FILES "${CMAKE_SOURCE_DIR}/data/kcp-power-96.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/96x96/apps RENAME kcp-power.png)

file(GLOB SVG_THEME_ICONS "./data/kcp-power*.svg")
install(FILES ${SVG_THEME_ICONS} DESTINATION ${INSTALL_DATADIR}/icons/hicolor/scalable/apps )