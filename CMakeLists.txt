cmake_minimum_required(VERSION 3.0)

project(kiran-cpanel-system)

option(DISABLE_LICENSE "Disable to license system " OFF)
option(DISABLE_SYSTEM_INFO "Don't show system and hardware information" OFF)
option(DISABLE_KIRANWIDGETS "Don't use kiran widgets" OFF)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools Network PrintSupport)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(LIBQRENCODE REQUIRED libqrencode)

if(NOT DISABLE_KIRANWIDGETS)
	pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
endif()

if(NOT DISABLE_SYSTEM_INFO)
	pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
	#通过kiran control panel的pkgconfig配置文件取出插件Desktop安装位置、插件共享库安装位置
	pkg_search_module(KIRAN_CONTROL_PANEL_PKG REQUIRED kiran-control-panel)
	pkg_get_variable(CPANEL_PLUGIN_DIR kiran-control-panel plugin_location)
	pkg_get_variable(CPANEL_DESKTOP_DIR kiran-control-panel plugin_desktop_location)
endif()


set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
set(BIN_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
set(TRANSLATIONS_DIR "${DATA_DIR}/${PROJECT_NAME}/translations/")
set(TS_FILES "${PROJECT_SOURCE_DIR}/translations/kiran-cpanel-system.zh_CN.ts" )

#添加config.h.in文件
configure_file (
"${PROJECT_SOURCE_DIR}/config/config.h.in"
"${PROJECT_BINARY_DIR}/config/config.h"
)

add_subdirectory(src)
add_subdirectory(data)
add_subdirectory(config)

