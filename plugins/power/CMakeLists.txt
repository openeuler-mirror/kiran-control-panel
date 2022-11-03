cmake_minimum_required(VERSION 3.2)

set(TARGET_NAME kiran-cpanel-power)

#add_definitions(-DTEST)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(QGSETTINGS REQUIRED gsettings-qt)
pkg_search_module(UPOWER_GLIB REQUIRED upower-glib)

file(GLOB_RECURSE SRC
        "src/*.cpp"
        "src/*.h"
        "src/*.ui")

file(GLOB_RECURSE QRC
        "resources/*.qrc")

set( POWER_TS_FILES "translations/kiran-cpanel-power.zh_CN.ts" )
qt5_create_translation( POWER_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${POWER_TS_FILES} )

add_library(${TARGET_NAME} SHARED
        ${SRC}
        ${QRC}
        ${POWER_QM_FILES})

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_CONTROL_PANEL_PKG_INCLUDE_DIRS}
        ${QGSETTINGS_INCLUDE_DIRS}
        ${UPOWER_GLIB_INCLUDE_DIRS}
        ${KCP_PLUGIN_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME}
        Qt5::Widgets
        Qt5::DBus
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KIRAN_CC_DAEMON_LIBRARIES}
        ${KLOG_QT5_LIBRARIES}
        ${QGSETTINGS_LIBRARIES}
        ${UPOWER_GLIB_LIBRARIES})

# 安装插件翻译
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
install(FILES ${POWER_QM_FILES} DESTINATION ${TRANSLATION_INSTALL_DIR})

# 安装插件
install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)