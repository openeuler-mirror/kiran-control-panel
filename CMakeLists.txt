cmake_minimum_required(VERSION 3.0...${CMAKE_VERSION})

project(kiran-control-panel VERSION 2.5)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include("options")
include("kcp_variables")
include("kcp-qdbus-wrapper")

configure_file(./config.h.in ${CMAKE_BINARY_DIR}/config.h)

# settings.ini 供 account/network 插件读取
configure_file(./settings.ini.in ${CMAKE_BINARY_DIR}/settings.ini)

# 服务器模式默认值由构建参数 SERVER_MODE 在构建期写入 gschema，仅为兼容性考虑：
# 让各形态的包自带正确的默认值，升级控制中心时不必依赖 kiran-desktop 等其它包同步升级。
# 定制化配置(例如统一调整系统形态)更推荐放到 kiran-desktop 的 override 机制，其优先级高于此默认值。
if(SERVER_MODE)
    set(KCP_SERVER_MODE_DEFAULT true)
else()
    set(KCP_SERVER_MODE_DEFAULT false)
endif()
configure_file(./data/com.kylinsec.kiran.control-panel.plugin.gschema.xml.in
        ${CMAKE_BINARY_DIR}/com.kylinsec.kiran.control-panel.plugin.gschema.xml @ONLY)

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
        MultimediaWidgets
        Network
        X11Extras
        Svg
        DBus
        PrintSupport
        Xml
        Concurrent
        LinguistTools)

find_package(PkgConfig REQUIRED)
pkg_search_module(KLOG REQUIRED klog-qt5)
pkg_search_module(KIRAN_WIDGETS REQUIRED kiranwidgets-qt5)
pkg_search_module(KIRAN_STYLE REQUIRED kiran-style-helper)
pkg_search_module(GLIB_2 REQUIRED glib-2.0)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(QGSETTINGS REQUIRED gsettings-qt)


file(GLOB_RECURSE COMMON_SRC "./common/*.cpp" "./common/*.h")
file(GLOB_RECURSE INCLUDE_SRC "./include/*.cpp" "./include/*.h")
file(GLOB_RECURSE CONTROL_PANEL_SRC "./src/*.cpp" "./src/*.h")
file(GLOB_RECURSE LIB_SRC "./lib/*.cpp" "./lib/*.c" "./lib/*.h")
file(GLOB_RECURSE PLUGINS_SRC "./plugins/*.cpp"  "./plugins/*.h")
file(GLOB_RECURSE LAUNCHER_SRC "./launcher/*.cpp" "./launcher/*.h")
#NOTE: 资源文件名不能与插件资源名相同，否则会导致插件中部分图片无法正常加载显示。
set(RESOURCE ./resources/control-panel-resources.qrc)

file(GLOB TS_FILES "translations/*.ts")
set(PANEL_ALL_TRANSLATION_SRC_DIR
        ${PROJECT_SOURCE_DIR}/common
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/launcher
        ${PROJECT_SOURCE_DIR}/lib
        ${PROJECT_SOURCE_DIR}/libexec
        ${PROJECT_SOURCE_DIR}/plugins
        ${PROJECT_SOURCE_DIR}/src)
qt5_create_translation(PANEL_QM_FILES ${PANEL_ALL_TRANSLATION_SRC_DIR} ${TS_FILES})

add_executable(${PROJECT_NAME}
        ${CONTROL_PANEL_SRC}
        ${COMMON_SRC}
        ${RESOURCE}
        ${PANEL_QM_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/include
        src
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_STYLE_INCLUDE_DIRS}
        ${GLIB_2_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
        common-widgets
        plugin-framework
        Qt5::Core
        Qt5::Gui
        Qt5::Svg
        Qt5::X11Extras
        Qt5::Widgets
        dl
        pthread
        ${KLOG_LIBRARIES}
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KIRAN_STYLE_LIBRARIES}
        ${GLIB_2_LIBRARIES})

install(FILES ${INCLUDE_SRC} DESTINATION ${KCP_INCLUDEDIR})
install(FILES ${PANEL_QM_FILES} DESTINATION ${TRANSLATION_DIR})
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_FULL_BINDDIR})

get_filename_component(KCP_SETTINGS_DIR ${KCP_SETTINGS} DIRECTORY CACHE)
install(FILES ${CMAKE_BINARY_DIR}/settings.ini DESTINATION ${KCP_SETTINGS_DIR})

add_subdirectory(lib)
add_subdirectory(libexec)
add_subdirectory(launcher)
add_subdirectory(plugins)
add_subdirectory(data)

if(BUILD_EXAMPLE)
        add_subdirectory(example)
endif()