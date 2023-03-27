set(TARGET_NAME kiran-cpanel-system)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools PrintSupport)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(KIRAN_STYLE_HELPER REQUIRED kiran-style-helper)

file(GLOB_RECURSE SYSTEM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/*.h
                             ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

file(GLOB TS_FILES "translations/*.ts")
qt5_create_translation(SYSTEM_QM ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

kiran_qt5_add_dbus_interface_ex(LICENSE_DBUS_SRC
        data/com.kylinsec.Kiran.LicenseObject.xml
        dbus_license_dbus
        DBusLicenseObject)

add_library(${TARGET_NAME} SHARED
        ${SYSTEM_SRC}
        ${SYSTEM_QM}
        ${LICENSE_DBUS_SRC}
        ${CMAKE_CURRENT_SOURCE_DIR}/data/res.qrc)

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KIRAN_STYLE_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::PrintSupport
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KLOG_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES})

install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_INSTALL_DIR})
install(FILES ${SYSTEM_QM} DESTINATION ${TRANSLATION_INSTALL_DIR}/)
