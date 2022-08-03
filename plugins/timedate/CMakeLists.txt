cmake_minimum_required(VERSION 3.5)

set(TARGET_NAME kiran-cpanel-timedate)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus Network LinguistTools)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(KIRAN_STYLE_HELPER REQUIRED kiran-style-helper)

file(GLOB_RECURSE SRC
        "src/*.cpp"
        "src/*.h")
        
include_directories("${CMAKE_BINARY_DIR}" "src" )
qt5_create_translation( TIMEDATE_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} "translations/kiran-cpanel-timedate.zh_CN.ts" )

add_library(${TARGET_NAME} SHARED
        ${SRC}
        ${QRC}
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
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::Network
        X11
        Xrandr
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES})

#安装插件Desktop
configure_file(config/kiran-cpanel-timedate.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-timedate.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-timedate.desktop DESTINATION ${PLUGIN_DESKTOP_INSTALL_DIR}/ )

#安装插件
install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_INSTALL_DIR})

#安装翻译文件
set(TRANSLATE_INSTALL_DIR ${INSTALL_DATADIR}/${TARGET_NAME}/translations/)
configure_file(./config/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/kcp-timedate-config.h @ONLY)
install(FILES ${TIMEDATE_QM_FILES} DESTINATION ${TRANSLATE_INSTALL_DIR} )
