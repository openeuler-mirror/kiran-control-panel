cmake_minimum_required(VERSION 3.5)

project(kiran-timedate-manager)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(kdtm-variables)

find_package(PkgConfig REQUIRED)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
find_package(Qt5 COMPONENTS Widgets Svg DBus Network LinguistTools)

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

set( TS_FILES "translations/${PROJECT_NAME}.zh_CN.ts" )
qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )
set(TRANSLATE_INSTALL_DIR "${KDTM_DATA_INSTALL_DIR}/${PROJECT_NAME}/translations/")

#若定义了插件Desktop文件目录，则编译成插件
if( DEFINED PLUGIN_DESKTOP_DIR )
    configure_file(./config/config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)
    include(plugin-build)
else()
    include(app-build)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
        include
        src/dbus-api-wrapper
        src/timezone
        src/widgets
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

install(FILES ${QM_FILES} DESTINATION ${TRANSLATE_INSTALL_DIR} )
