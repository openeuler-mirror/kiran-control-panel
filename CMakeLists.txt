cmake_minimum_required(VERSION 3.0)

project(kiran-control-panel VERSION 2.5)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS 
        Core 
        Gui 
        Widgets
        Multimedia 
        Network 
        X11Extras 
        Svg  
        DBus 
        PrintSupport
        Xml
        Concurrent
        LinguistTools)

pkg_search_module(KLOG REQUIRED klog-qt5)
pkg_search_module(KIRAN_WIDGETS REQUIRED kiranwidgets-qt5)
pkg_search_module(KIRAN_STYLE REQUIRED kiran-style-helper)
pkg_search_module(GLIB_2 REQUIRED glib-2.0)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(QGSETTINGS REQUIRED gsettings-qt)

include(kcp_variables.cmake)
include(kcp-qdbus-wrapper.cmake)
include(options.cmake)

configure_file(${CMAKE_SOURCE_DIR}/data/config.h.in ${CMAKE_BINARY_DIR}/config.h)
configure_file(${CMAKE_SOURCE_DIR}/data/${PROJECT_NAME}.desktop.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop)
configure_file(${CMAKE_SOURCE_DIR}/data/${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)

file(GLOB_RECURSE INCLUDE_SRC "./include/*")
file(GLOB_RECURSE CONTROL_PANEL_SRC "./src/*")
file(GLOB_RECURSE LIB_SRC "./lib/*")
set(RESOURCE ./resources/control-panel-resources.qrc) #资源文件名不能与插件资源名相同，否则会导致插件中部分图片无法正常加载显示。

file(GLOB TS_FILES "translations/*.ts")
set(PANEL_ALL_TRANSLATION_SRC ${INCLUDE_SRC} ${CONTROL_PANEL_SRC} ${LIB_SRC} )
qt5_create_translation(PANEL_QM_FILES ${PANEL_ALL_TRANSLATION_SRC} ${TS_FILES})

add_executable(${PROJECT_NAME}
        ${CONTROL_PANEL_SRC}
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

list(FILTER INCLUDE_SRC EXCLUDE REGEX ".*bus-tray-monitor.h$")
install(FILES "${CMAKE_SOURCE_DIR}/data/kiran-control-panel.svg" DESTINATION ${INSTALL_DATADIR}/icons/hicolor/)
install(FILES ${INCLUDE_SRC} DESTINATION ${KCP_INSTALL_INCLUDE}/)
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop" DESTINATION "${INSTALL_DATADIR}/applications/")
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION "${INSTALL_LIBDIR}/pkgconfig/")
install(FILES ${PANEL_QM_FILES} DESTINATION ${TRANSLATION_INSTALL_DIR})
install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_BINDIR}/)

file(GLOB CATEGORY_DESKTOP "./data/category/desktop/*.desktop")

if(NOT ENABLE_NETWORK)
    list(FILTER CATEGORY_DESKTOP EXCLUDE REGEX "network.desktop")
endif()

if(NOT ENABLE_AUDIO)
    list(FILTER CATEGORY_DESKTOP EXCLUDE REGEX "audio.desktop")
endif()

if(NOT ENABLE_USER_GROUP)
    list(FILTER CATEGORY_DESKTOP EXCLUDE REGEX "group.desktop")
endif()

install(FILES ${CATEGORY_DESKTOP} DESTINATION ${CATEGORY_DESKTOP_INSTALL_DIR})

file(GLOB CATEGORY_ICONS "./data/category/images/*")
install(FILES ${CATEGORY_ICONS} DESTINATION ${CATEGORY_ICON_INSTALL_DIR})

add_subdirectory(lib)
add_subdirectory(launcher)
add_subdirectory(plugins)
add_subdirectory(data)
