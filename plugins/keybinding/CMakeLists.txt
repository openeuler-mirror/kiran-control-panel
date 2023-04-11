set(TARGET_NAME kiran-cpanel-keybinding)

file(GLOB TS_FILES "translation/*.ts")
qt5_create_translation(KEYBINDING_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
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
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::Core
        Qt5::Concurrent
        ${KIRANWIDGETS_LIBRARIES}
        ${KLOG_LIBRARIES})

install(TARGETS ${TARGET_NAME}
        DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)

install(FILES ${KEYBINDING_QM_FILES}
        DESTINATION ${TRANSLATION_INSTALL_DIR}/)
