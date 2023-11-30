set (TARGET_NAME dbus-wrapper)

set(DBUS_WRAPPER_GEN_CODE "")
file(GLOB_RECURSE DBUS_WRAPPER_SRC "./*.cpp" "./*.h")

# com.kylinsec.Kiran.SystemDaemon.Accounts
kiran_qt5_add_dbus_interface_ex(KIRAN_ACCOUNT_SERVICE
        com.kylinsec.Kiran.SystemDaemon.Accounts.xml
        kiran_account_service
        KiranAccountService)
list(APPEND DBUS_WRAPPER_GEN_CODE ${KIRAN_ACCOUNT_SERVICE})

kiran_qt5_add_dbus_interface_ex(KIRAN_ACCOUNT_SERVICE_USER
        com.kylinsec.Kiran.SystemDaemon.Accounts.User.xml
        kiran_account_service_user
        KiranAccountServiceUser)
list(APPEND DBUS_WRAPPER_GEN_CODE ${KIRAN_ACCOUNT_SERVICE_USER})

# com.kylinsec.Kiran.SessionDaemon.Appearance
kiran_qt5_add_dbus_interface_ex(KIRAN_APPEARANCE_SERVICE
        com.kylinsec.Kiran.SessionDaemon.Appearance.xml
        kiran_appearance_service
        KiranAppearanceService)
list(APPEND DBUS_WRAPPER_GEN_CODE ${KIRAN_APPEARANCE_SERVICE})

add_library(${TARGET_NAME} STATIC
        ${COMMON_SRC}
        ${DBUS_WRAPPER_GEN_CODE}
        ${DBUS_WRAPPER_SRC} )

target_link_libraries(${TARGET_NAME}
        Qt5::Core
        Qt5::DBus
        ${KLOG_LIBRARIES})

target_include_directories(${TARGET_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/common
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KLOG_INCLUDE_DIRS})

target_include_directories(${TARGET_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR})