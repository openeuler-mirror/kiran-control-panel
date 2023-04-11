cmake_minimum_required(VERSION 3.2)

set(TARGET_NAME kiran-cpanel-power)

#add_definitions(-DTEST)

pkg_search_module(UPOWER_GLIB REQUIRED upower-glib)

file(GLOB_RECURSE POWER_SRC
        "src/*.cpp"
        "src/*.h"
        "src/*.ui")

file(GLOB_RECURSE POWER_QRC
        "resources/*.qrc")

file(GLOB POWER_TS_FILES "translations/*.ts")
qt5_create_translation( POWER_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${POWER_TS_FILES} )

add_library(${TARGET_NAME} SHARED
        ${POWER_SRC}
        ${POWER_QRC}
        ${POWER_QM_FILES})

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_CONTROL_PANEL_PKG_INCLUDE_DIRS}
        ${QGSETTINGS_INCLUDE_DIRS}
        ${UPOWER_GLIB_INCLUDE_DIRS}
        ${KCP_PLUGIN_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME}
        Qt5::Widgets
        Qt5::DBus
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KIRAN_CC_DAEMON_LIBRARIES}
        ${KLOG_LIBRARIES}
        ${QGSETTINGS_LIBRARIES}
        ${UPOWER_GLIB_LIBRARIES})

# 安装插件翻译
install(FILES ${POWER_QM_FILES} DESTINATION ${TRANSLATION_INSTALL_DIR})

# 安装插件
install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)
