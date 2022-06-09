cmake_minimum_required(VERSION 3.2)

set (TARGET_NAME common-widgets)

find_package(Qt5 COMPONENTS Core Widgets LinguistTools)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KIRAN_STYLE_HELPER REQUIRED kiran-style-helper)

file(GLOB_RECURSE COMMON_WIDGETS_INFO_H_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE COMMON_WIDGETS_INFO_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(${TARGET_NAME} STATIC ${COMMON_WIDGETS_INFO_H_FILES} ${COMMON_WIDGETS_INFO_CPP_FILES})

target_include_directories(${TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS})

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_BINARY_DIR}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        Qt5::Core
        Qt5::Widgets
        plugin-info
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES})