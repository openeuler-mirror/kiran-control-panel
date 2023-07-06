set(TARGET_NAME kiran-cpanel-mouse)

option(MOUSE_WHEEL_TEST_VISIBLE "Is mouse wheel direction test visible" OFF)

file(GLOB_RECURSE MOUSE_SRC "./*.cpp" "./*.h" "./*.ui" "./*.qrc" )

file(GLOB TS_FILES "translations/*.ts")
qt5_create_translation( MOUSE_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

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
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::Core
        ${KIRANWIDGETS_LIBRARIES}
        ${KLOG_LIBRARIES})

if (MOUSE_WHEEL_TEST_VISIBLE)
    add_definitions(-DMOUSE_WHEEL_DIRECTION_TEST)
endif ()

install(TARGETS ${TARGET_NAME}
        DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)

install(FILES ${MOUSE_QM_FILES} DESTINATION ${TRANSLATION_INSTALL_DIR}/)
