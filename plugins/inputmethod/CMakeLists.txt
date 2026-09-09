set(TARGET_NAME kiran-cpanel-inputmethod)

file(GLOB_RECURSE INPUTMETHOD_SRC "./*.cpp" "./*.h" "./*.ui" "./*.qrc")

# ---- D-Bus proxy 生成（qdbusxml2cpp）----
# fcitx4 的 D-Bus 访问走 XML 生成的 proxy（替代手搓 Properties.Get/Set）。
# 类型映射头 fcitx4/fcitx4-dbus-types.h 由 XML 的 QtTypeName 注解引用，-i 引入。
# 注意：生成文件名不含连字符（qdbusxml2cpp 用文件名拼 include guard，含连字符
# 会产生非法宏名警告）；GetConfig 只保留第一个 v out（动态值树保持手搓解析）。
find_program(QDBUSXML2CPP_EXECUTABLE
             NAMES qdbusxml2cpp-qt5 qdbusxml2cpp
             PATHS "${_qt5_bin_dir}" "/usr/lib64/qt5/bin" "/usr/lib/qt5/bin")

if(NOT QDBUSXML2CPP_EXECUTABLE)
    message(FATAL_ERROR "qdbusxml2cpp not found (install qt5-qtbase-devel)")
endif()

set(_DBUS_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/dbus-generated")

add_custom_command(
    OUTPUT ${_DBUS_GEN_DIR}/fcitx4inputmethodproxy.h ${_DBUS_GEN_DIR}/fcitx4inputmethodproxy.cpp
    COMMAND ${CMAKE_COMMAND} -E make_directory ${_DBUS_GEN_DIR}
    COMMAND ${QDBUSXML2CPP_EXECUTABLE} -N -i ${CMAKE_CURRENT_SOURCE_DIR}/fcitx4/fcitx4-dbus-types.h
            -p ${_DBUS_GEN_DIR}/fcitx4inputmethodproxy -c Fcitx4InputMethodProxy
            ${CMAKE_CURRENT_SOURCE_DIR}/data/org.fcitx.Fcitx.InputMethod.xml
    DEPENDS data/org.fcitx.Fcitx.InputMethod.xml fcitx4/fcitx4-dbus-types.h
    COMMENT "Generating fcitx4 D-Bus proxy")

add_library(${TARGET_NAME} SHARED
        ${INPUTMETHOD_SRC}
        ${_DBUS_GEN_DIR}/fcitx4inputmethodproxy.cpp)

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}          # 插件根 = 唯一插件内 include 根（规范路径）
        ${CMAKE_CURRENT_BINARY_DIR}          # ui_im-page.h 生成物
        ${_DBUS_GEN_DIR}                     # fcitx4 proxy 生成物
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
