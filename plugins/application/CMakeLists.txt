set(TARGET_NAME kiran-cpanel-application)

pkg_search_module(Qt5Xdg REQUIRED Qt5Xdg)
pkg_search_module(GLIB_2 REQUIRED glib-2.0)
find_package(KF5Config REQUIRED)

file(GLOB_RECURSE APPLICATION_PLUGIN_SRC
                  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

set(DEFAULTAPP_INCLUDE_DIRS src src/pages ${CMAKE_SOURCE_DIR}/lib/common-widgets/kiran-setting-container)

add_library(${TARGET_NAME} SHARED
        ${DEFAULT_SRC})

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KIRAN_STYLE_INCLUDE_DIRS}
        ${PROJECT_BINARY_DIR}
        ${DEFAULTAPP_INCLUDE_DIRS}
        ${Qt5Xdg_INCLUDE_DIRS}
        ${GLIB_2_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::PrintSupport
        KF5::ConfigCore
        KF5::ConfigCore
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KLOG_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES}
        ${Qt5Xdg_LIBRARIES}
        ${GLIB_2_LIBRARIES})

install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_DIR}/)