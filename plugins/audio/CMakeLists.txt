set(TARGET_NAME kiran-cpanel-audio)

file(GLOB_RECURSE AUDIO_TRANSLATION_SRC ./*.cpp ./*.h ./*.ui)
file(GLOB TS_FILES "translations/*.ts")
qt5_create_translation(AUDIO_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES} ${AUDIO_TRANSLATION_SRC})

file(GLOB_RECURSE COMMON_SRC "src/audio-node.h" "src/volume-slider.cpp" "src/volume-slider.h")
file(GLOB_RECURSE DBUS_SRC "src/dbus/*.cpp" "src/dbus/*.h")
file(GLOB_RECURSE PLUGIN_SRC "src/plugin/*.cpp" "src/plugin/*.h" "src/plugin/*.ui")
file(GLOB_RECURSE SYSTEM_TRAY_SRC "src/system-tray/*.cpp" "src/system-tray/*.h" "src/system-tray/*.ui")
file(GLOB_RECURSE QRC "resources/*.qrc")

add_library(${TARGET_NAME} SHARED
        ${COMMON_SRC}
        ${DBUS_SRC}
        ${PLUGIN_SRC}
        ${QRC}
        ${AUDIO_QM_FILES})

target_include_directories(${TARGET_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/common
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}
        src
        src/plugin
        src/dbus
        config
        resources
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KLOG_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        Qt5::Widgets Qt5::DBus Qt5::Svg Qt5::Multimedia
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KIRAN_CC_DAEMON_LIBRARIES}
        ${KLOG_LIBRARIES})

## 托盘区域-音量托盘
set(TRAY_PROCESS kiran-audio-status-icon)

add_executable(${TRAY_PROCESS}
        ${COMMON_SRC}
        ${DBUS_SRC}
        ${SYSTEM_TRAY_SRC}
        ${AUDIO_QM_FILES}
        ${QRC}
        ${PROJECT_SOURCE_DIR}/include/dbus-tray-monitor.h)

target_include_directories(${TRAY_PROCESS} PRIVATE
        ${CMAKE_BINARY_DIR}
        src
        src/system-tray
        src/dbus
        config
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_STYLE_HELPER_DIRS})

target_link_libraries(${TRAY_PROCESS}
        common-widgets
        Qt5::Widgets Qt5::DBus Qt5::Svg Qt5::Multimedia
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KIRAN_CC_DAEMON_LIBRARIES}
        ${KLOG_LIBRARIES}
        ${KIRAN_STYLE_LIBRARIES})

#安装翻译
install(FILES ${AUDIO_QM_FILES} DESTINATION ${TRANSLATION_DIR})

set(AUTOSTART_DESKTOP_DIR /etc/xdg/autostart)
#安装插件desktop文件
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/kiran-audio-status-icon.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/kiran-audio-status-icon.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-audio-status-icon.desktop DESTINATION ${AUTOSTART_DESKTOP_DIR}/)

#安装插件和二进制文件
install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_DIR}/)
install(TARGETS ${TRAY_PROCESS} DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

file(GLOB SVG_THEME_ICONS "./resources/kcp-audio-images/kcp-audio*.svg")
install(FILES ${SVG_THEME_ICONS} DESTINATION ${INSTALL_DATADIR}/icons/hicolor/scalable/apps)
