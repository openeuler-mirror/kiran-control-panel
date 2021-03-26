cmake_minimum_required(VERSION 3.5)

project(kiran-account-manager)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(ZERO_MQ REQUIRED libzmq)

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
        ${ZERO_MQ_INCLUDE_DIRS})

target_link_libraries(kiran-account-manager
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        pam
        crypt
        kiranwidgets-qt5
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KIRAN_CC_DAEMON_LIBRARIES}
        ${ZERO_MQ_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)

install(FILES ${QM_FILES}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/translations/ )

install(FILES "./config/kiran-account-manager.desktop"
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/applications/ )

file(GLOB BUILDIN_AVATARS "./buildin-avatar/*")
install(FILES ${BUILDIN_AVATARS}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/account-icons/ )

add_subdirectory(avatar-editor)