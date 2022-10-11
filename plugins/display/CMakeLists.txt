cmake_minimum_required(VERSION 3.0)

set(TARGET_NAME kiran-cpanel-display)

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
pkg_search_module(KIRAN_STYLE_HELPER REQUIRED kiran-style-helper)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h" "src/*.ui")

qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} translations/kiran-cpanel-display.zh_CN.ts)

add_library(${TARGET_NAME} SHARED ${SRC} ${QM_FILES})

target_include_directories(${TARGET_NAME} PRIVATE
        src/
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRANWIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CONTROL_PANEL_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::DBus
        Qt5::X11Extras
        Qt5::Svg
        ${KIRAN_STYLE_HELPER_LIBRARIES})

#安装插件到指定目录
install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)

#安装插件desktop
configure_file(data/kiran-cpanel-display.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-display.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-display.desktop DESTINATION ${PLUGIN_DESKTOP_INSTALL_DIR}/ )

#安装翻译文件
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/data/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
install(FILES ${QM_FILES} DESTINATION ${TRANSLATION_INSTALL_DIR} )