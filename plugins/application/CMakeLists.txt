set(TARGET_NAME kiran-cpanel-application)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools PrintSupport)
find_package(KF5Config REQUIRED)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(KIRAN_STYLE_HELPER REQUIRED kiran-style-helper)
pkg_search_module(Qt5Xdg REQUIRED Qt5Xdg)
pkg_search_module(GLIB_2 REQUIRED glib-2.0)

file(GLOB_RECURSE DEFAULT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/*.h
                             ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

file(GLOB TS_FILES "translations/*.ts")
qt5_create_translation(DEFAULT_QM ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})


set(DEFAULTAPP_INCLUDE_DIRS src src/defaultapp src/autostart ${CMAKE_SOURCE_DIR}/lib/common-widgets/kiran-setting-container)


add_library(${TARGET_NAME} SHARED
        ${DEFAULT_SRC}
        ${DEFAULT_QM}
        ${GLOB_RECURSE}
        )


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
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KLOG_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES}
        ${Qt5Xdg_LIBRARIES}
        ${GLIB_2_LIBRARIES})

install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)
install(FILES ${DEFAULT_QM} DESTINATION ${TRANSLATION_INSTALL_DIR}/)

