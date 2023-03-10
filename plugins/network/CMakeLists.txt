set(TARGET_NAME kiran-cpanel-network)

qt5_create_translation(NETWORK_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} translations/kiran-cpanel-network.zh_CN.ts)
file(GLOB_RECURSE PLUGIN_SRC "src/plugin/*.cpp" "src/plugin/*.h" "src/plugin/*.ui")
file(GLOB_RECURSE TRAY_SRC "src/tray/*.cpp" "src/tray/*.h" "src/tray/*.ui")
file(GLOB_RECURSE COMMON_SRC "src/animation-loading-label*" "src/connection-list*"
 "src/status-notification*" "src/utils*" "src/signal-forward*" "src/general.h")
file(GLOB_RECURSE QRC "resources/*.qrc")

add_library(${TARGET_NAME} SHARED
        ${PLUGIN_SRC}
        ${COMMON_SRC}
        ${QRC}
        ${NETWORK_QM_FILES})

find_package(KF5NetworkManagerQt REQUIRED)
pkg_search_module(LIBNOTIFY REQUIRED libnotify)

target_include_directories(${TARGET_NAME} PRIVATE
        src
        src/plugin
        src/plugin/manager
        src/plugin/settings
        src/plugin/settings/vpn
        src/plugin/setting-widget
        src/plugin/setting-widget/vpn
        src/plugin/details-page
        resources
        data
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${CMAKE_BINARY_DIR}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KLOG_INCLUDE_DIRS}
        ${LIBNOTIFY_INCLUDE_DIRS}
        ${KIRAN_STYLE_INCLUDE_DIRS}
        )

target_link_libraries(${TARGET_NAME}
        common-widgets
        ${REQUIRED_LIBS_QUALIFIED}
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES}
        ${LIBNOTIFY_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES}
        KF5::NetworkManagerQt
        )
      
set(TRAY_PROCESS kiran-network-status-icon)

add_executable(${TRAY_PROCESS}
        ${TRAY_SRC}
        ${PLUGIN_SRC}
        ${COMMON_SRC}
        ${QRC}
        ${NETWORK_QM_FILES}
        )

target_include_directories(${TRAY_PROCESS} PRIVATE
        src
        src/plugin
        src/plugin/manager
        src/plugin/settings
        src/plugin/settings/vpn
        src/plugin/setting-widget
        src/plugin/setting-widget/vpn
        src/tray
        src/tray/dbus
        resources
        data
        ${CMAKE_BINARY_DIR}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${LIBNOTIFY_INCLUDE_DIRS}
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS}
        )

target_link_libraries(${TRAY_PROCESS}
        common-widgets
        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::DBus Qt5::Network Qt5::Svg
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KLOG_LIBRARIES}
        ${LIBNOTIFY_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES}
        KF5::NetworkManagerQt
        )

#安装翻译
install(FILES ${NETWORK_QM_FILES} DESTINATION ${TRANSLATION_INSTALL_DIR}/)

set(AUTOSTART_DESKTOP_DIR /etc/xdg/autostart)
#安装插件desktop文件
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/kiran-network-status-icon.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/kiran-network-status-icon.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-network-status-icon.desktop DESTINATION ${AUTOSTART_DESKTOP_DIR}/)

set(NETWORK_SERVER_CONF /etc/NetworkManager/conf.d)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/00-server.conf.in ${CMAKE_CURRENT_BINARY_DIR}/00-server.conf @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/00-server.conf DESTINATION ${NETWORK_SERVER_CONF}/)

#安装插件和二进制文件
install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)
install(TARGETS ${TRAY_PROCESS} DESTINATION ${INSTALL_BINDIR})

file(GLOB SVG_THEME_ICONS "./resources/kcp-network-images/*.svg")
install(FILES ${SVG_THEME_ICONS} DESTINATION ${INSTALL_DATADIR}/icons/hicolor/scalable/apps)
