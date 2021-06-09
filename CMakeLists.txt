cmake_minimum_required(VERSION 3.0)

project(kiran-cpanel-mouse)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools Network)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)

#通过kiran control panel的pkgconfig配置文件取出插件Desktop安装位置、插件共享库安装位置
pkg_search_module(KIRAN_CONTROL_PANEL_PKG REQUIRED kiran-control-panel)
pkg_get_variable(CPANEL_PLUGIN_DIR kiran-control-panel plugin_location)
pkg_get_variable(CPANEL_DESKTOP_DIR kiran-control-panel plugin_desktop_location)


set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")

set(TRANSLATIONS_DIR "${DATA_DIR}/${PROJECT_NAME}/translation/")
set( TS_FILES "translations/kiran-cpanel-mouse.zh_CN.ts" )
set( INCLUDE_PATH "/usr/include/glib-2.0/" "/usr/lib64/glib-2.0/include/" "/usr/include" "/usr/include/kiranwidgets-qt5" "/usr/include/kiran-cc-daemon")

#添加config.h.in文件
configure_file (
"${PROJECT_SOURCE_DIR}/config/config.h.in"
"${PROJECT_BINARY_DIR}/config/config.h"
)

#定义源文件和头文件
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE FORMS "src/*.ui")
file(GLOB_RECURSE RESOURCES "resources/*.qrc")


include_directories("${CMAKE_BINARY_DIR}" "src" "${INCLUDE_PATH}" 
						 "${KIRAN_CONTROL_PANEL_PKG_INCLUDE_DIRS}" 
						 "${KLOG_QT5_INCLUDE_DIRS}")

qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )


add_library(${PROJECT_NAME} SHARED
				${SOURCES}
				${HEADERS}
				${FORMS}
				${RESOURCES}
				${QM_FILES})
		
target_link_libraries(${PROJECT_NAME} 
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
	Qt5::Core
        kiranwidgets-qt5
        ${KIRANWIDGETS_LIBRARIES}
	${KLOG_QT5_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CPANEL_PLUGIN_DIR}/)

install(FILES ${QM_FILES}
        DESTINATION ${TRANSLATIONS_DIR})

add_subdirectory(config)
add_subdirectory(resources/images)

