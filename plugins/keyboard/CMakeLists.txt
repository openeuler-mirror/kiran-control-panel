cmake_minimum_required(VERSION 3.0)

set(TARGET_NAME kiran-cpanel-keyboard)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools Network)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(KIRAN_STYLE_HELPER REQUIRED kiran-style-helper)

qt5_create_translation(KEYBOARD_QM ${CMAKE_CURRENT_SOURCE_DIR} translation/kiran-cpanel-keyboard.zh_CN.ts)

kiran_qt5_add_dbus_interface_ex(KEYBOARD_PROXY
        data/com.kylinsec.Kiran.SessionDaemon.Keyboard.xml
        keyboard_backEnd_proxy
        KeyboardBackEndProxy)

file(GLOB_RECURSE KEYBOARD_SRC "./*.cpp" "./*.h" "./*.ui" "./*.qrc")

add_library(${TARGET_NAME} SHARED
        ${KEYBOARD_SRC}
        ${KEYBOARD_PROXY}
        ${KEYBOARD_QM})

target_include_directories(${TARGET_NAME} PRIVATE
        ./src/
        ./src/pages/general
        ./src/pages/layout
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::Core
        ${KIRANWIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES})

install(TARGETS ${TARGET_NAME}
        DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)

install(FILES ${KEYBOARD_QM}
        DESTINATION ${TRANSLATION_INSTALL_DIR}/)
