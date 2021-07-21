cmake_minimum_required(VERSION 3.5)

project(kiran-cpanel-account)

#账户信息页面，认证管理入口是否显示
option(AUTH_MANAGER_VISIBLE "Is auth manager visible" OFF)
#认证管理页面是否显示人脸认证相关
option(FACE_AUTH_VISIBLE "face auth visible" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(kam-variables)
#配置文件安装路径
set(KAM_CONFIG_PATH "${KAM_CONF_DIR}/kiran-account-manager.conf")
#默认头像路径
set(KAM_DEFAULT_AVATAR_PATH "${KAM_BUILDIN_AVATAR_DIR_PATH}/0.face")

configure_file(${CMAKE_SOURCE_DIR}/config/config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(ZERO_MQ REQUIRED libzmq)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)

#通过kiran control panel的pkgconfig配置文件取出插件Desktop安装位置、插件共享库安装位置
pkg_search_module(KIRAN_CONTROL_PANEL_PKG REQUIRED kiran-control-panel)
pkg_get_variable(CPANEL_PLUGIN_DIR kiran-control-panel plugin_location)
pkg_get_variable(CPANEL_DESKTOP_DIR kiran-control-panel plugin_desktop_location)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE FORMS "src/*.ui")
file(GLOB_RECURSE TOOLS_SOURCES "tools/*.cpp")
file(GLOB_RECURSE TOOLS_HEADERS "tools/*.h")
file(GLOB_RECURSE RESOURCES "resources/*.qrc")
set( TS_FILES "translations/kiran-cpanel-account.zh_CN.ts" )

include_directories("${CMAKE_BINARY_DIR}" "src" "src/pages/" "src/widgets" )

qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )


add_library(${PROJECT_NAME} SHARED
        ${SOURCES}
        ${HEADERS}
        ${TOOLS_SOURCES}
        ${TOOLS_HEADERS}
        ${RESOURCES}
        ${QM_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        include
        src/dbus-api-wrapper
        src/pages
        src/widgets
        tools
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${ZERO_MQ_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_CONTROL_PANEL_PKG_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        pam
        crypt
        kiranwidgets-qt5
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KIRAN_CC_DAEMON_LIBRARIES}
        ${ZERO_MQ_LIBRARIES}
        ${KLOG_QT5_LIBRARIES})

if( AUTH_MANAGER_VISIBLE )
    add_definitions(-DAUTH_MANAGER)
endif()

if( FACE_AUTH_VISIBLE )
    add_definitions(-DFACE_AUTH)
endif()

#安装控制中心插件
install(TARGETS ${PROJECT_NAME} DESTINATION ${CPANEL_PLUGIN_DIR}/ )
#安装控制中心插件desktop
configure_file(${CMAKE_SOURCE_DIR}/config/kiran-cpanel-account.desktop.in ${CMAKE_BINARY_DIR}/kiran-cpanel-account.desktop @ONLY )
install(FILES ${CMAKE_BINARY_DIR}/kiran-cpanel-account.desktop  DESTINATION ${CPANEL_DESKTOP_DIR}/ )

SET(link_source ${CPANEL_DESKTOP_DIR}/${PROJECT_NAME}.desktop)
SET(link_target ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-link.desktop)
add_custom_target(kcp-applications-desktop ALL COMMAND ln -sf ${link_source} ${link_target})
install(FILES ${link_target}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/applications/
        RENAME ${PROJECT_NAME}.desktop)

#安装翻译文件
install(FILES ${QM_FILES} DESTINATION ${KAM_TRANSLATION_DIR_PATH} )
#安装账户管理配置文件
install(FILES ./config/kiran-account-manager.conf DESTINATION ${KAM_CONF_DIR})
#安装内置头像
file(GLOB BUILDIN_AVATARS "./buildin-avatar/*")
install(FILES ${BUILDIN_AVATARS} DESTINATION ${KAM_BUILDIN_AVATAR_DIR_PATH} )
#安装账户管理图标
set(KCP_ICON_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor)
install(FILES "${CMAKE_SOURCE_DIR}/resources/kcp-account-images/kcp-account-24.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/24x24/apps RENAME kcp-account.png)
install(FILES "${CMAKE_SOURCE_DIR}/resources/kcp-account-images/kcp-account-32.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/32x32/apps RENAME kcp-account.png)
install(FILES "${CMAKE_SOURCE_DIR}/resources/kcp-account-images/kcp-account-48.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/48x48/apps RENAME kcp-account.png)
install(FILES "${CMAKE_SOURCE_DIR}/resources/kcp-account-images/kcp-account-64.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/64x64/apps RENAME kcp-account.png)
install(FILES "${CMAKE_SOURCE_DIR}/resources/kcp-account-images/kcp-account-96.png" DESTINATION ${KCP_ICON_INSTALL_PATH}/96x96/apps RENAME kcp-account.png)
add_subdirectory(avatar-editor)