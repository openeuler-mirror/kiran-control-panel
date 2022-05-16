cmake_minimum_required(VERSION 3.2)

set(TARGET_NAME kiran-cpanel-appearance)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools Xml Concurrent Network)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)

set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
set(TRANSLATIONS_DIR "${DATA_DIR}/${TARGET_NAME}/translations/")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config/config.h)

kiran_qt5_add_dbus_interface_ex(APPEARANCE_PROXY
        src/dbus-interface/com.kylinsec.Kiran.SessionDaemon.Appearance.xml
        appearance_backEnd_proxy AppearanceBackEndProxy)

qt5_create_translation( APPEARANCE_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} translations/kiran-cpanel-appearance.zh_CN.ts )

file(GLOB_RECURSE APPEARANCE_SOURCE "*.cpp" "*.h")
file(GLOB_RECURSE APPEARANCE_FORMS "*.ui")
file(GLOB_RECURSE APPEARANCE_QRC "*.qrc")

add_library(${TARGET_NAME} SHARED
        ${APPEARANCE_SOURCE}
        ${APPEARANCE_FORMS}
        ${APPEARANCE_QRC}
        ${APPEARANCE_PROXY}
        ${APPEARANCE_QM_FILES})

target_include_directories(${TARGET_NAME} PRIVATE
        src
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KCP_PLUGIN_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME}
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::Core
        Xcursor
        X11
        ${KIRANWIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES})

install(TARGETS ${TARGET_NAME}
        DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)

install(FILES ${QM_FILES}
        DESTINATION ${TRANSLATIONS_DIR})

add_subdirectory(config)
add_subdirectory(resources)
