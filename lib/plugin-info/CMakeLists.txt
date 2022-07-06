cmake_minimum_required(VERSION 3.2)

set (TARGET_NAME plugin-info)

find_package(Qt5 COMPONENTS Core Widgets LinguistTools)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(GLIB_2 REQUIRED glib-2.0)

file(GLOB_RECURSE PLUGIN_INFO_H_FILES ./*.h)
file(GLOB_RECURSE PLUGIN_INFO_CPP_FILES ./*.cpp)

add_library(${TARGET_NAME} STATIC ${PLUGIN_INFO_H_FILES} ${PLUGIN_INFO_CPP_FILES})

target_include_directories(${TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include)

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_BINARY_DIR}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${GLIB_2_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME} PRIVATE
        Qt5::Core
        Qt5::Widgets
        ${KLOG_QT5_LIBRARIES}
        ${GLIB_2_LIBRARIES})

