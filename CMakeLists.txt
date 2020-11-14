cmake_minimum_required(VERSION 3.5)

project(kiran-timedate-manager)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PkgConfig REQUIRED)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE FORMS "src/*.ui")
file(GLOB_RECURSE TOOLS_SOURCES "tools/*.cpp")
file(GLOB_RECURSE TOOLS_HEADERS "tools/*.h")
file(GLOB_RECURSE RESOURCES "resources/*.qrc")
set( TS_FILES "translations/${PROJECT_NAME}.zh_CN.ts" )

include_directories("${CMAKE_BINARY_DIR}" "src" )

find_package(Qt5 COMPONENTS Widgets Svg DBus Network LinguistTools)

qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )

add_executable(${PROJECT_NAME}
        ${SOURCES}
        ${HEADERS}
        ${TOOLS_SOURCES}
        ${TOOLS_HEADERS}
        ${RESOURCES}
        ${QM_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        include
        src/dbus-api-wrapper
        src/timezone
        src/widgets
        src/libs/SingleApplication
        tools
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::Network
        X11
        Xrandr
        kiranwidgets-qt5
        ${KIRAN_WIDGETS_QT5_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)

install(FILES ${QM_FILES}
        DESTINATION ${SHARE_INSTALL_PREFIX}/${PROJECT_NAME}/translations/ )

install(FILES "./config/${PROJECT_NAME}.desktop"
        DESTINATION ${SHARE_INSTALL_PREFIX}/applications/ )
