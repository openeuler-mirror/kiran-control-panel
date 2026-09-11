set(TARGET_NAME kiran-cpanel-inputmethod)

file(GLOB_RECURSE INPUTMETHOD_SRC "./*.cpp" "./*.h" "./*.ui" "./*.qrc")

# 显式生成 Fcitx4 D-Bus 代理及 moc，避免生成头滞后导致元对象代码缺失。
set(FCITX4_DBUS_XML data/org.fcitx.Fcitx.InputMethod.xml)
set_source_files_properties(${FCITX4_DBUS_XML}
        PROPERTIES
        INCLUDE fcitx4/fcitx4-dbus-types.h)
kiran_qt5_add_dbus_interface_ex(FCITX4_DBUS_PROXY
        ${FCITX4_DBUS_XML}
        fcitx4inputmethodproxy
        Fcitx4InputMethodProxy)

add_library(${TARGET_NAME} SHARED
        ${INPUTMETHOD_SRC}
        ${FCITX4_DBUS_PROXY})

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}          # 插件根 = 唯一插件内 include 根（规范路径）
        ${CMAKE_CURRENT_BINARY_DIR}          # ui_im-page.h 生成物
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_STYLE_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::Core
        Qt5::DBus
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KLOG_LIBRARIES}
        ${KIRAN_STYLE_LIBRARIES})

install(TARGETS ${TARGET_NAME}
        DESTINATION ${PLUGIN_LIBS_DIR}/)
