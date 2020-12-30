cmake_minimum_required(VERSION 3.5)

project(kiran-avatar-editor)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(kam-variables)

find_package(PkgConfig REQUIRED)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE TOOLS_SOURCES "tools/*.cpp")
file(GLOB_RECURSE TOOLS_HEADERS "tools/*.h")
file(GLOB_RECURSE FORMS "src/*.ui")
file(GLOB_RECURSE RESOURCES "*.qrc")

set( TS_FILES "translations/kiran-avatar-editor.zh_CN.ts" )

include_directories("${CMAKE_BINARY_DIR}" "src")

find_package(Qt5 COMPONENTS Widgets LinguistTools)
qt5_create_translation( AVATAR_EDITOR_QM_FILES ${CMAKE_SOURCE_DIR}/avatar-editor ${TS_FILES} )

add_executable(kiran-avatar-editor ${SOURCES} ${HEADERS} ${TOOLS_SOURCES} ${TOOLS_HEADERS} ${RESOURCES} ${AVATAR_EDITOR_QM_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE
        include
        tools
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS})
target_link_libraries(kiran-avatar-editor
        Qt5::Widgets
        ${KIRAN_WIDGETS_QT5_LIBRARIES})

target_include_directories(${PROJECT_NAME} PRIVATE
        include
        tools
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS})

target_link_libraries(kiran-avatar-editor
        Qt5::Widgets
        ${KIRAN_WIDGETS_QT5_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION ${KAM_AVATAR_EDITOR_DIR})
install(FILES ${AVATAR_EDITOR_QM_FILES} DESTINATION ${KAM_TRANSLATION_DIR_PATH} )
