cmake_minimum_required(VERSION 3.0)

set(TARGET_NAME kiran-cpanel-keybinding)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools Network)

pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)

set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
set(TRANSLATIONS_DIR "${DATA_DIR}/${PROJECT_NAME}/translations/")

#添加config.h.in文件
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/keybinding-config.h)

file(GLOB_RECURSE KEYBINDING_SRC "./*.cpp" "./*.h" "./*.ui" "./*.qrc")
qt5_create_translation(KEYBINDING_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/translation/kiran-cpanel-keybinding.zh_CN.ts)
kiran_qt5_add_dbus_interface_ex(KEYBINDING_PROXY
        data/com.kylinsec.Kiran.SessionDaemon.Keybinding.xml
        keybinding_backEnd_proxy
        KeybindingBackEndProxy)

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
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::Core
        ${KIRANWIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES})

install(TARGETS ${TARGET_NAME}
        DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)

install(FILES ${QM_FILES}
        DESTINATION ${TRANSLATIONS_DIR})

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/data/kiran-cpanel-keybinding.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-keybinding.desktop)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-keybinding.desktop" DESTINATION ${PLUGIN_DESKTOP_INSTALL_DIR}/ )
SET(link_source ${PLUGIN_DESKTOP_INSTALL_DIR}/kiran-cpanel-keybinding.desktop)
add_custom_target(kiran-cpanel-keybinding-desktop ALL
        COMMAND ln -sf ${link_source} ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-keybinding.desktop)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-keybinding.desktop"
        DESTINATION ${DATA_DIR}/applications/)

add_subdirectory(resources/images)

