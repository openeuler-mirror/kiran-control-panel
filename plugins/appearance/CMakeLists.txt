cmake_minimum_required(VERSION 3.2)

set(TARGET_NAME kiran-cpanel-appearance)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools Xml Concurrent Network)

pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(KIRAN_STYLE_HELPER REQUIRED kiran-style-helper)

set(APPEARANCE_TRANSLATIONS_DIR ${INSTALL_DATADIR}/${TARGET_NAME}/translations/)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/appearance-config.h.in ${CMAKE_CURRENT_BINARY_DIR}/appearance-config.h)

kiran_qt5_add_dbus_interface_ex(APPEARANCE_PROXY
        src/dbus-interface/com.kylinsec.Kiran.SessionDaemon.Appearance.xml
        appearance_backEnd_proxy AppearanceBackEndProxy)

qt5_create_translation( APPEARANCE_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} translations/kiran-cpanel-appearance.zh_CN.ts )

file(GLOB_RECURSE APPEARANCE_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

add_library(${TARGET_NAME} SHARED ${APPEARANCE_SRC} ${APPEARANCE_PROXY})

target_include_directories(${TARGET_NAME} PRIVATE
        src
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS}
        ${KCP_PLUGIN_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::Core
        Xcursor
        X11
        ${KIRANWIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES})

install(TARGETS ${TARGET_NAME}
        DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)

install(FILES ${QM_FILES}
        DESTINATION ${APPEARANCE_TRANSLATIONS_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kiran-cpanel-appearance.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-appearance.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-appearance.desktop DESTINATION ${PLUGIN_DESKTOP_INSTALL_DIR}/)

SET(link_source ${PLUGIN_DESKTOP_INSTALL_DIR}/${TARGET_NAME}.desktop)
SET(link_target ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}-link.desktop)
add_custom_target(kcp-appearance-desktop ALL COMMAND ln -sf ${link_source} ${link_target})
install(FILES ${link_target}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/applications/
        RENAME ${TARGET_NAME}.desktop)
