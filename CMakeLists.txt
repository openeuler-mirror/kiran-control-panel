cmake_minimum_required(VERSION 3.0)

project(kiran-cpanel-system)

option(ENABLE_LICENSE "Is commit code to openEuler " OFF)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools Network PrintSupport)
find_package(KiranDBusGenerate REQUIRED)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(LIBQRENCODE REQUIRED libqrencode)

#通过kiran control panel的pkgconfig配置文件取出插件Desktop安装位置、插件共享库安装位置
pkg_search_module(KIRAN_CONTROL_PANEL_PKG REQUIRED kiran-control-panel)
pkg_get_variable(CPANEL_PLUGIN_DIR kiran-control-panel plugin_location)
pkg_get_variable(CPANEL_DESKTOP_DIR kiran-control-panel plugin_desktop_location)


set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
set(TRANSLATIONS_DIR "${DATA_DIR}/${PROJECT_NAME}/translations/")
set( TS_FILES "${PROJECT_SOURCE_DIR}/translations/kiran-cpanel-system.zh_CN.ts" )

#添加config.h.in文件
configure_file (
"${PROJECT_SOURCE_DIR}/config/config.h.in"
"${PROJECT_BINARY_DIR}/config/config.h"
)

#set( INCLUDE_PATH "/usr/include/glib-2.0/" "/usr/lib64/glib-2.0/include/" "/usr/include" "/usr/include/kylin-license" "/usr/include/kiran-cc-daemon" "/usr/include/kiranwidgets-qt5/")

if(ENABLE_LICENSE)
	add_definitions(-DENABLE_LICENSE)
endif()

add_subdirectory(src)
add_subdirectory(data)
add_subdirectory(config)

