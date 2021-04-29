cmake_minimum_required(VERSION 3.5)

project(kiran-account-manager)
#账户信息页面，认证管理入口是否显示
option(AUTH_MANAGER_VISIBLE "Is auth manager visible" OFF)
#认证管理页面是否显示人脸认证相关
option(FACE_AUTH_VISIBLE "face auth visible" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(kam-variables)
configure_file(${CMAKE_SOURCE_DIR}/config/config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/config/kiran-account-manager.desktop.in ${CMAKE_BINARY_DIR}/kiran-account-manager.desktop @ONLY )

find_package(PkgConfig REQUIRED)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(ZERO_MQ REQUIRED libzmq)
pkg_search_module(ZLOG REQUIRED zlog)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE FORMS "src/*.ui")
file(GLOB_RECURSE TOOLS_SOURCES "tools/*.cpp")
file(GLOB_RECURSE TOOLS_HEADERS "tools/*.h")
file(GLOB_RECURSE RESOURCES "resources/*.qrc")
set( TS_FILES "translations/kiran-account-manager.zh_CN.ts" )

include_directories("${CMAKE_BINARY_DIR}" "src" "src/pages/" "src/widgets" )

find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools)

qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )

add_executable(kiran-account-manager
        ${SOURCES}
        ${HEADERS}
        ${TOOLS_SOURCES}
        ${TOOLS_HEADERS}
        ${RESOURCES}
        ${QM_FILES})

target_include_directories(kiran-account-manager PRIVATE
        include
        src/dbus-api-wrapper
        src/pages
        src/widgets
        tools
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${ZERO_MQ_INCLUDE_DIRS}
        ${ZLOG_INCLUDE_DIRS})

target_link_libraries(kiran-account-manager
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        pam
        crypt
        kiranwidgets-qt5
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KIRAN_CC_DAEMON_LIBRARIES}
        ${ZERO_MQ_LIBRARIES}
        ${ZLOG_LIBRARIES})

if( AUTH_MANAGER_VISIBLE )
    add_definitions(-DAUTH_MANAGER)
endif()

if( FACE_AUTH_VISIBLE )
    add_definitions(-DFACE_AUTH)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${KAM_INSTALL_DIR} )
install(FILES ${QM_FILES} DESTINATION ${KAM_TRANSLATION_DIR_PATH} )
install(FILES ${CMAKE_BINARY_DIR}/kiran-account-manager.desktop  DESTINATION ${KAM_DESKTOP_INSTALL_DIR} )

file(GLOB BUILDIN_AVATARS "./buildin-avatar/*")
install(FILES ${BUILDIN_AVATARS} DESTINATION ${KAM_BUILDIN_AVATAR_DIR_PATH} )
install(FILES "${CMAKE_SOURCE_DIR}/resources/images/account-management.png" DESTINATION ${KAM_DATA_DIR})

add_subdirectory(avatar-editor)