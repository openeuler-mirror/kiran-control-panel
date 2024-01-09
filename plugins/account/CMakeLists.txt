set(TARGET_NAME kiran-cpanel-account)

#账户信息页面，密码过期策略是否显示
option(PASSWD_EXPIRATION_POLICY_VISIBLE "Is password expiration policy visible" OFF)

set(KCP_ACCOUNT_CONF_DIR ${SYSCONFDIR}/${TARGET_NAME})
set(KCP_ACCOUNT_DATA_DIR ${INSTALL_DATADIR}/${TARGET_NAME})
set(KCP_ACCOUNT_BUILDIN_AVATAR_DIR ${KCP_ACCOUNT_DATA_DIR}/account-icons)
set(KCP_ACCOUNT_AVATAR_EDITOR ${INSTALL_LIBEXEC})
set(KCP_ACCOUNT_AVATAR_EDITOR_PATH ${KCP_ACCOUNT_AVATAR_EDITOR}/kiran-avatar-editor)
set(KCP_ACCOUNT_CONFIG_PATH ${KCP_ACCOUNT_DATA_DIR}/kiran-account-manager.conf)
set(KCP_ACCOUNT_DEFAULT_AVATAR_PATH ${KCP_ACCOUNT_BUILDIN_AVATAR_DIR}/0.face)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/kcp-account-config.h @ONLY)

pkg_search_module(CRYPTOPP REQUIRED cryptopp)
pkg_search_module(ZERO_MQ REQUIRED libzmq)

file(GLOB TS_FILES "translations/*.ts")
qt5_create_translation(ACCOUNT_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

file(GLOB_RECURSE ACCOUNT_SRC
        "src/*.cpp"
        "src/*.h"
        "src/*.ui"
        "tools/*.cpp"
        "tools/*.h")

set(ACCOUNT_DBUS_SRC_LIST "")
kiran_qt5_add_dbus_interface_ex(KSD_ACCOUNTS_SRC
        data/com.kylinsec.Kiran.SystemDaemon.Accounts.xml
        ksd_accounts_proxy
        KSDAccountsProxy)
list(APPEND ACCOUNT_DBUS_SRC_LIST ${KSD_ACCOUNTS_SRC})

kiran_qt5_add_dbus_interface_ex(KSD_ACCOUNTS_USER_SRC
        data/com.kylinsec.Kiran.SystemDaemon.Accounts.User.xml
        ksd_accounts_user_proxy
        KSDAccountsUserProxy)
list(APPEND ACCOUNT_DBUS_SRC_LIST ${KSD_ACCOUNTS_USER_SRC})

kiran_qt5_add_dbus_interface_ex(KSD_BIOMETRICS_SRC
        data/com.kylinsec.Kiran.SystemDaemon.Biometrics.xml
        ksd_biometrics_proxy
        KSDBiometricsProxy)
list(APPEND ACCOUNT_DBUS_SRC_LIST ${KSD_BIOMETRICS_SRC})

add_library(${TARGET_NAME} SHARED
        ${ACCOUNT_SRC}
        ${ACCOUNT_QM_FILES}
        ${ACCOUNT_DBUS_SRC_LIST})

target_include_directories(${TARGET_NAME} PRIVATE
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${CMAKE_BINARY_DIR}
        include
        src
        src/dbus-api-wrapper
        src/pages
        src/widgets
        tools
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${ZERO_MQ_INCLUDE_DIRS}
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_STYLE_INCLUDE_DIRS}
        ${CRYPTOPP_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        pam
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KIRAN_CC_DAEMON_LIBRARIES}
        ${ZERO_MQ_LIBRARIES}
        ${KLOG_LIBRARIES}
        ${KIRAN_STYLE_LIBRARIES}
        ${CRYPTOPP_LIBRARIES})

if (PASSWD_EXPIRATION_POLICY_VISIBLE)
    add_definitions(-DPASSWD_EXPIRATION_POLICY)
endif ()

install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)

install(FILES ${ACCOUNT_QM_FILES} DESTINATION ${TRANSLATION_INSTALL_DIR})

install(FILES ./data/kiran-account-manager.conf DESTINATION ${KCP_ACCOUNT_CONF_DIR})

file(GLOB BUILDIN_AVATARS "./buildin-avatar/*")
install(FILES ${BUILDIN_AVATARS} DESTINATION ${KCP_ACCOUNT_BUILDIN_AVATAR_DIR})

add_subdirectory(avatar-editor)
