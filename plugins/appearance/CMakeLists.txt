set(TARGET_NAME kiran-cpanel-appearance)

kiran_qt5_add_dbus_interface_ex(APPEARANCE_PROXY
        src/dbus-interface/com.kylinsec.Kiran.SessionDaemon.Appearance.xml
        appearance_backEnd_proxy AppearanceBackEndProxy)
 
qt5_create_translation( APPEARANCE_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} "translations/kiran-cpanel-appearance.zh_CN.ts" )

file(GLOB_RECURSE APPEARANCE_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

add_library(${TARGET_NAME} SHARED ${APPEARANCE_SRC} ${APPEARANCE_PROXY} ${APPEARANCE_QM_FILES})

target_include_directories(${TARGET_NAME} PRIVATE
        src
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS}
        ${KCP_PLUGIN_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::Core
        Xcursor
        X11
        ${KIRANWIDGETS_LIBRARIES}
        ${KLOG_LIBRARIES}
        ${KIRAN_STYLE_LIBRARIES})

install(TARGETS ${TARGET_NAME}
        DESTINATION ${PLUGIN_LIBS_INSTALL_DIR}/)

install(FILES ${APPEARANCE_QM_FILES}
        DESTINATION ${TRANSLATION_INSTALL_DIR})
