set(TARGET_NAME kiran-cpanel-keybinding)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools Concurrent)

pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)

qt5_create_translation(KEYBINDING_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/translation/kiran-cpanel-keybinding.zh_CN.ts)
kiran_qt5_add_dbus_interface_ex(KEYBINDING_PROXY
        data/com.kylinsec.Kiran.SessionDaemon.Keybinding.xml
        keybinding_backEnd_proxy
        KeybindingBackEndProxy)

file(GLOB_RECURSE KEYBINDING_SRC "./*.cpp" "./*.h" "./*.ui")

add_library(${TARGET_NAME} SHARED
        ${KEYBINDING_SRC}
        ${KEYBINDING_QM_FILES}
        ${KEYBINDING_PROXY})

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::Core
        Qt5::Concurrent
        ${KIRANWIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES})

install(TARGETS ${TARGET_NAME}
        DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)

install(FILES ${KEYBINDING_QM_FILES}
        DESTINATION ${TRANSLATION_INSTALL_DIR}/)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/data/kiran-cpanel-keybinding.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-keybinding.desktop)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-keybinding.desktop DESTINATION ${PLUGIN_DESKTOP_INSTALL_DIR}/)