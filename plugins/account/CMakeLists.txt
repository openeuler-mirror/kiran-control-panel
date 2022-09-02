cmake_minimum_required(VERSION 3.2)

set(TARGET_NAME kiran-cpanel-account)

#账户信息页面，密码过期策略是否显示
option(PASSWD_EXPIRATION_POLICY_VISIBLE "Is password expiration policy visible" OFF)

set(KCP_ACCOUNT_CONF_DIR ${SYSCONFDIR}/${TARGET_NAME})

set(KCP_ACCOUNT_DATA_DIR ${INSTALL_DATADIR}/${TARGET_NAME})
set(KCP_ACCOUNT_BUILDIN_AVATAR_DIR ${KCP_ACCOUNT_DATA_DIR}/account-icons)
set(KCP_ACCOUNT_TRANSLATION_DIR ${KCP_ACCOUNT_DATA_DIR}/translations)

set(KCP_ACCOUNT_AVATAR_EDITOR ${INSTALL_LIBEXEC})
set(KCP_ACCOUNT_AVATAR_EDITOR_PATH ${KCP_ACCOUNT_AVATAR_EDITOR}/kiran-avatar-editor)

set(KCP_ACCOUNT_CONFIG_PATH ${KCP_ACCOUNT_DATA_DIR}/kiran-account-manager.conf)
set(KCP_ACCOUNT_DEFAULT_AVATAR_PATH ${KCP_ACCOUNT_BUILDIN_AVATAR_DIR}/0.face)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/kcp-account-config.h @ONLY)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KIRAN_STYLE_HELPER REQUIRED kiran-style-helper)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(ZERO_MQ REQUIRED libzmq)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)

qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} translations/kiran-cpanel-account.zh_CN.ts)

file(GLOB_RECURSE ALL_SOURCES
        "src/*.cpp"
        "src/*.h"
        "src/*.ui"
        "tools/*.cpp"
        "tools/*.h")

#DBus代理代码生成
set(DBUS_SRC_LIST "")
kiran_qt5_add_dbus_interface_ex(KSD_ACCOUNTS_SRC
        data/com.kylinsec.Kiran.SystemDaemon.Accounts.xml
        ksd_accounts_proxy
        KSDAccountsProxy)
list(APPEND DBUS_SRC_LIST ${KSD_ACCOUNTS_SRC})

kiran_qt5_add_dbus_interface_ex(KSD_ACCOUNTS_USER_SRC
        data/com.kylinsec.Kiran.SystemDaemon.Accounts.User.xml
        ksd_accounts_user_proxy
        KSDAccountsUserProxy)
list(APPEND DBUS_SRC_LIST ${KSD_ACCOUNTS_USER_SRC})

kiran_qt5_add_dbus_interface_ex(KSD_BIOMETRICS_SRC
        data/com.kylinsec.Kiran.SystemDaemon.Biometrics.xml
        ksd_biometrics_proxy
        KSDBiometricsProxy)
list(APPEND DBUS_SRC_LIST ${KSD_BIOMETRICS_SRC})

add_library(${TARGET_NAME} SHARED
        ${ALL_SOURCES}
        ${QM_FILES}
        ${DBUS_SRC_LIST})

target_include_directories(${TARGET_NAME} PRIVATE
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${CMAKE_BINARY_DIR}
        include
        src
        src/dbus-api-wrapper
        src/pages
        src/widgets
        tools
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${ZERO_MQ_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        pam
        crypt
        kiranwidgets-qt5
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KIRAN_CC_DAEMON_LIBRARIES}
        ${ZERO_MQ_LIBRARIES}
        ${KLOG_QT5_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES})

if (PASSWD_EXPIRATION_POLICY_VISIBLE)
    add_definitions(-DPASSWD_EXPIRATION_POLICY)
endif ()

install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/kiran-cpanel-account.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-account.desktop @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-account.desktop DESTINATION ${PLUGIN_DESKTOP_INSTALL_DIR}/)

install(FILES ${QM_FILES} DESTINATION ${KCP_ACCOUNT_TRANSLATION_DIR})

install(FILES ./data/kiran-account-manager.conf DESTINATION ${KCP_ACCOUNT_CONF_DIR})

file(GLOB BUILDIN_AVATARS "./buildin-avatar/*")
install(FILES ${BUILDIN_AVATARS} DESTINATION ${KCP_ACCOUNT_BUILDIN_AVATAR_DIR})

add_subdirectory(avatar-editor)
