cmake_minimum_required(VERSION 3.0)

set(TARGET_NAME kiran-cpanel-mouse)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools Network)
find_package(KiranDBusGenerate REQUIRED)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)


file(GLOB_RECURSE MOUSE_SRC "./*.cpp" "./*.h" "./*.ui" "./*.qrc" )

qt5_create_translation( MOUSE_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/translations/kiran-cpanel-mouse.zh_CN.ts )

kiran_qt5_add_dbus_interface_ex(MOUSE_PROXY
        data/com.kylinsec.Kiran.SessionDaemon.Mouse.xml
        mouse_backEnd_proxy
        MouseBackEndProxy
        )

kiran_qt5_add_dbus_interface_ex(TOUCHPAD_PROXY
        data/com.kylinsec.Kiran.SessionDaemon.TouchPad.xml
        touchPad_backEnd_proxy
        TouchPadBackEndProxy
        )

add_library(${TARGET_NAME} SHARED
        ${MOUSE_SRC}
        ${MOUSE_QM_FILES}
        ${MOUSE_PROXY}
        ${TOUCHPAD_PROXY})

target_include_directories(${TARGET_NAME} PRIVATE
        ./src
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::Core
        ${KIRANWIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES})

install(TARGETS ${TARGET_NAME}
        DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)

install(FILES ./data/kiran-cpanel-mouse.desktop
        DESTINATION ${PLUGIN_DESKTOP_INSTALL_DIR}/)

install(FILES ${MOUSE_QM_FILES} DESTINATION ${TRANSLATION_INSTALL_DIR}/)