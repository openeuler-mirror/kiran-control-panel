set(TARGET_NAME kiran-cpanel-authentication)

pkg_search_module(KIRAN_AUTH_SERVICE REQUIRED kiran-authentication-service)
set(AUTH_TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/translations/kiran-cpanel-authentication.zh_CN.ts)
qt5_create_translation(AUTH_QM_FILES ${AUTH_TS_FILES} ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE AUTHENTICATION_SRC
        "src/*.cpp"
        "src/*.h"
        "src/*.ui")

kiran_qt5_add_dbus_interface_ex(KRAN_AUTHENTICATION_SRC
        data/com.kylinsec.Kiran.Authentication.xml
        kas_auth_proxy
        KASAuthProxy)
list(APPEND AUTH_DBUS_SRC_LIST ${KRAN_AUTHENTICATION_SRC})

kiran_qt5_add_dbus_interface_ex(KRAN_AUTHENTICATION_USER_SRC
        data/com.kylinsec.Kiran.Authentication.User.xml
        kas_auth_user_proxy
        KASAuthUserProxy)
list(APPEND AUTH_DBUS_SRC_LIST ${KRAN_AUTHENTICATION_USER_SRC})

add_library(${TARGET_NAME} SHARED
        ${AUTH_DBUS_SRC_LIST}
        ${AUTHENTICATION_SRC}
        ${AUTH_QM_FILES})
# add_executable(${TARGET_NAME} ${AUTHENTICATION_SRC} ${AUTH_QM_FILES})

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${PROJECT_BINARY_DIR}
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_AUTH_SERVICE_INCLUDE_DIRS}
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_STYLE_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KLOG_LIBRARIES}
        ${KIRAN_STYLE_LIBRARIES}
        ${KIRAN_AUTH_SERVICE_LIBRARIES})

install(TARGETS ${TARGET_NAME}
        DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)

install(FILES ${AUTH_QM_FILES} DESTINATION ${TRANSLATION_INSTALL_DIR})