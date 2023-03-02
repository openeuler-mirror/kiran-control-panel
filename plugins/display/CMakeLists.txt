set(TARGET_NAME kiran-cpanel-display)

file(GLOB_RECURSE DISPLAY_SRC "src/*.cpp" "src/*.h" "src/*.ui")

qt5_create_translation(DISPLAY_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} translations/kiran-cpanel-display.zh_CN.ts)

kiran_qt5_add_dbus_interface_ex(DISPLAY_PROXY
        data/com.kylinsec.Kiran.SessionDaemon.Display.xml
        display_backEnd_proxy
        DisplayBackEndProxy
        )

set(INTERFACE_MONITOR_XML_FILES data/com.kylinsec.Kiran.SessionDaemon.Display.Monitor.xml)
set_property(SOURCE ${INTERFACE_MONITOR_XML_FILES} PROPERTY INCLUDE generic-types.h)
kiran_qt5_add_dbus_interface_ex(MONITOR_PROXY
        ${INTERFACE_MONITOR_XML_FILES}
        monitor_backEnd_proxy
        MonitorBackEndProxy
)

add_library(${TARGET_NAME} SHARED 
        ${DISPLAY_SRC} 
        ${DISPLAY_QM_FILES} 
        ${DISPLAY_PROXY} 
        ${MONITOR_PROXY}
        )

target_include_directories(${TARGET_NAME} PRIVATE
        ${PROJECT_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        src/
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${KIRAN_STYLE_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::DBus
        Qt5::X11Extras
        Qt5::Svg
        ${KIRAN_STYLE_LIBRARIES}
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KLOG_LIBRARIES})

#安装插件到指定目录
install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)

#安装插件desktop
configure_file(data/kiran-cpanel-display.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-display.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-display.desktop DESTINATION ${PLUGIN_DESKTOP_INSTALL_DIR}/ )

#安装翻译文件
install(FILES ${QM_FILES} DESTINATION ${TRANSLATION_INSTALL_DIR} )