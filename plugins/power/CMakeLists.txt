cmake_minimum_required(VERSION 3.2)

set(TARGET_NAME kiran-cpanel-power)

if (SERVER_MODE)
    add_definitions(-DSERVER_MODE)
endif ()

pkg_search_module(UPOWER_GLIB REQUIRED upower-glib)

set(POWER_PLUGIN_DBUS_SRC_LIST "")

# com.kylinsec.Kiran.SessionDaemon.Power
set(KSD_POWER_XML data/com.kylinsec.Kiran.SessionDaemon.Power.xml)
set_source_files_properties(${KSD_POWER_XML}
        PROPERTIES
        INCLUDE dbus/idle-action.h)
kiran_qt5_add_dbus_interface_ex(KSD_POWER_SRC
        ${KSD_POWER_XML}
        ksd_power_proxy
        KSDPowerProxy)
list(APPEND POWER_PLUGIN_DBUS_SRC_LIST ${KSD_POWER_SRC})

# org.gnome.SessionManager.xml - kiran-session-manager
set(GNOME_SESSION_MANAGER_XML data/org.gnome.SessionManager.xml)
kiran_qt5_add_dbus_interface_ex(GNOME_SESSION_MANAGER_SOUCE
        ${GNOME_SESSION_MANAGER_XML}
        gnome_session_manager_proxy
        GnomeSMProxy)
list(APPEND POWER_PLUGIN_DBUS_SRC_LIST ${GNOME_SESSION_MANAGER_SOUCE})


file(GLOB_RECURSE POWER_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

add_library(${TARGET_NAME} SHARED
        ${POWER_SRC}
        ${POWER_PLUGIN_DBUS_SRC_LIST})

target_include_directories(${TARGET_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/common
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KLOG_INCLUDE_DIRS}
        ${QGSETTINGS_INCLUDE_DIRS}
        ${UPOWER_GLIB_INCLUDE_DIRS}
        ${KIRAN_STYLE_INCLUDE_DIRS}
        ${KCP_PLUGIN_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME}
        Qt5::Widgets
        Qt5::DBus
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KIRAN_CC_DAEMON_LIBRARIES}
        ${KIRAN_STYLE_LIBRARIES}
        ${KLOG_LIBRARIES}
        ${QGSETTINGS_LIBRARIES}
        ${UPOWER_GLIB_LIBRARIES})

# 安装插件
install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_DIR}/)
