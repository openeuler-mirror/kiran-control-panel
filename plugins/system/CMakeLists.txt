set(TARGET_NAME kiran-cpanel-system)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(KIRAN_STYLE_HELPER REQUIRED kiran-style-helper)

file(GLOB_RECURSE SYSTEM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/*.h
                             ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

qt5_create_translation(SYSTEM_QM ${CMAKE_CURRENT_SOURCE_DIR} .//translations/kiran-cpanel-system.zh_CN.ts)

add_library(${TARGET_NAME} SHARED
        ${SYSTEM_SRC}
        ${SYSTEM_QM}
        ${CMAKE_CURRENT_SOURCE_DIR}/data/res.qrc)

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES})

install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_INSTALL_DIR})
install(FILES ${SYSTEM_QM} DESTINATION ${INSTALL_DATADIR}/${TARGET_NAME}/translations/)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}.desktop DESTINATION ${PLUGIN_DESKTOP_INSTALL_DIR}/)