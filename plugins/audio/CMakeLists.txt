set(TARGET_NAME kiran-cpanel-audio)

## 控制中心-音量插件
find_package(Qt5 COMPONENTS Widgets DBus Svg Multimedia)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_STYLE_HELPER REQUIRED kiran-style-helper)

qt5_create_translation(AUDIO_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} translations/kiran-cpanel-audio.zh_CN.ts)

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
        src
        src/plugin
        src/dbus
        config
        resources
        ${CMAKE_BINARY_DIR}
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        Qt5::Widgets Qt5::DBus Qt5::Svg Qt5::Multimedia
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KIRAN_CC_DAEMON_LIBRARIES}
        ${KLOG_QT5_LIBRARIES})

## 托盘区域-音量托盘
set(TRAY_PROCESS kiran-audio-status-icon)

add_executable(${TRAY_PROCESS}
        ${COMMON_SRC}
        ${DBUS_SRC}
        ${SYSTEM_TRAY_SRC}
        ${AUDIO_QM_FILES}
        ${QRC})

target_include_directories(${TRAY_PROCESS} PRIVATE
        ${CMAKE_BINARY_DIR}
        src
        src/system-tray
        src/dbus
        config
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_STYLE_HELPER_DIRS})

target_link_libraries(${TRAY_PROCESS}
        common-widgets
        Qt5::Widgets Qt5::DBus Qt5::Svg Qt5::Multimedia
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KIRAN_CC_DAEMON_LIBRARIES}
        ${KLOG_QT5_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES})

#安装翻译
set(AUDIO_TRANSLATION_DIR_INSTALL_PATH ${INSTALL_DATADIR}/${TARGET_NAME}/translations)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
install(FILES ${AUDIO_QM_FILES} DESTINATION ${AUDIO_TRANSLATION_DIR_INSTALL_PATH})

set(AUTOSTART_DESKTOP_DIR /etc/xdg/autostart)
#安装插件desktop文件
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/kiran-cpanel-audio.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-audio.desktop @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/kiran-audio-status-icon.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/kiran-audio-status-icon.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-audio.desktop DESTINATION ${PLUGIN_DESKTOP_INSTALL_DIR}/)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-audio-status-icon.desktop DESTINATION ${AUTOSTART_DESKTOP_DIR}/)

SET(link_source ${PLUGIN_DESKTOP_INSTALL_DIR}/${TARGET_NAME}.desktop)
SET(link_target ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}-link.desktop)
add_custom_target(kcp-applications-desktop ALL COMMAND ln -sf ${link_source} ${link_target})
install(FILES ${link_target}
        DESTINATION ${INSTALL_DATADIR}/applications/
        RENAME ${TARGET_NAME}.desktop)

#安装插件和二进制文件
install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)
install(TARGETS ${TRAY_PROCESS} DESTINATION ${INSTALL_BINDIR})

file(GLOB SVG_THEME_ICONS "./resources/kcp-audio-images/kcp-audio*.svg")
install(FILES ${SVG_THEME_ICONS} DESTINATION ${INSTALL_DATADIR}/icons/hicolor/scalable/apps)
