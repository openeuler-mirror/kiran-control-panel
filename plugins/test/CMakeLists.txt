cmake_minimum_required(VERSION 3.5)

set(TARGET_NAME kiran-cpanel-test)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus Network LinguistTools)

pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(KIRAN_INTEGRATION_THEME REQUIRED kiran-integration-theme)

file(GLOB_RECURSE SRC
        "src/*.cpp"
        "src/*.h")

include_directories("${CMAKE_BINARY_DIR}" "src" )

add_library(${TARGET_NAME} SHARED
        ${SRC}
        ${TIMEDATE_QM_FILES})

target_include_directories(${TARGET_NAME} PRIVATE
        include
        src/dbus-api-wrapper
        src/timezone
        src/widgets
        src/tools
        src/pages
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_CONTROL_PANEL_PKG_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KIRAN_INTEGRATION_THEME_INCLUDE_DIRS})

#安装插件
install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_DIR})