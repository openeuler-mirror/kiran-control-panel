cmake_minimum_required(VERSION 3.5)

project(kiran-avatar-editor)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE TOOLS_SOURCES "tools/*.cpp")
file(GLOB_RECURSE TOOLS_HEADERS "tools/*.h")
file(GLOB_RECURSE FORMS "src/*.ui")
file(GLOB_RECURSE RESOURCES "*.qrc")

set( TS_FILES "translations/kiran-avatar-editor.zh_CN.ts" )

include_directories("${CMAKE_BINARY_DIR}" "src")

find_package(Qt5 COMPONENTS Widgets LinguistTools)

qt5_create_translation( AVATAR_EDITOR_QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )
add_executable(kiran-avatar-editor ${SOURCES} ${HEADERS} ${TOOLS_SOURCES} ${TOOLS_HEADERS} ${RESOURCES} ${AVATAR_EDITOR_QM_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE include tools )
target_link_libraries(kiran-avatar-editor Qt5::Widgets kiranwidgets-qt5)
