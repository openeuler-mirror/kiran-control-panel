cmake_minimum_required(VERSION 3.0)

set(TARGET_NAME kiran-cpanel-keyboard)

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools Network)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)

set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
set(TRANSLATIONS_DIR "${DATA_DIR}/${PROJECT_NAME}/translations/")
set(TS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/translation/kiran-cpanel-keyboard.zh_CN.ts" )

#添加config.h.in文件
configure_file (
${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.in
${CMAKE_CURRENT_BINARY_DIR}/config/config.h
)

add_subdirectory(src)
add_subdirectory(config)
add_subdirectory(resources/images)

