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

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h" "src/*.ui")
file(GLOB_RECURSE QRC "resources/*.qrc")

qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} translations/kiran-cpanel-display.zh_CN.ts)

add_library(${TARGET_NAME} SHARED ${SRC} ${QRC} ${QM_FILES})

target_include_directories(${TARGET_NAME} PRIVATE
        src/
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRANWIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CONTROL_PANEL_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KCP_PLUGIN_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME}
        Qt5::Widgets
        Qt5::DBus
        Qt5::X11Extras
        Qt5::Svg)

#安装插件到指定目录
install(TARGETS ${TARGET_NAME} DESTINATION ${CPANEL_PLUGIN_DIR}/)

#安装插件desktop
configure_file(data/kiran-cpanel-display.desktop.in ${CMAKE_BINARY_DIR}/kiran-cpanel-display.desktop @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/kiran-cpanel-display.desktop DESTINATION ${CPANEL_DESKTOP_DIR}/ )

SET(link_source ${PLUGIN_DESKTOP_INSTALL_DIR}/${TARGET_NAME}.desktop)
SET(link_target ${CMAKE_BINARY_DIR}/${TARGET_NAME}-link.desktop)
add_custom_target(kcp-applications-account-desktop ALL COMMAND ln -sf ${link_source} ${link_target})
install(FILES ${link_target}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/applications/
        RENAME ${TARGET_NAME}.desktop)

#安装插件图标
set(KCP_ICON_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/data/kcp-display-24.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/24x24/apps RENAME kcp-display.png)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/data/kcp-display-32.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/32x32/apps RENAME kcp-display.png)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/data/kcp-display-48.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/48x48/apps RENAME kcp-display.png)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/data/kcp-display-64.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/64x64/apps RENAME kcp-display.png)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/data/kcp-display-96.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/96x96/apps RENAME kcp-display.png)

#安装翻译文件
set(TRANSLATION_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/${TARGET_NAME}/translations)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/data/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
install(FILES ${QM_FILES} DESTINATION ${TRANSLATION_INSTALL_DIR} )