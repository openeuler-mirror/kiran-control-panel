cmake_minimum_required(VERSION 3.0)

project(kiran-cpanel-system)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools Network PrintSupport)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(LIBQRENCODE REQUIRED libqrencode)

#通过kiran control panel的pkgconfig配置文件取出插件Desktop安装位置、插件共享库安装位置
pkg_search_module(KIRAN_CONTROL_PANEL_PKG REQUIRED kiran-control-panel)
pkg_get_variable(CPANEL_PLUGIN_DIR kiran-control-panel plugin_location)
pkg_get_variable(CPANEL_DESKTOP_DIR kiran-control-panel plugin_desktop_location)


set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")

set(TARGET_NAME ${PROJECT_NAME})
set(TRANSLATIONS_DIR "${DATA_DIR}/${PROJECT_NAME}/translations/")
set( TS_FILES "translations/kiran-cpanel-system.zh_CN.ts" )
set( INCLUDE_PATH "/usr/include/glib-2.0/" "/usr/lib64/glib-2.0/include/" "/usr/include" "/usr/include/kylin-license" "/usr/include/kiran-cc-daemon" "/usr/include/kiranwidgets-qt5/")

#添加config.h.in文件
configure_file (
"${PROJECT_SOURCE_DIR}/config/config.h.in"
"${PROJECT_BINARY_DIR}/config/config.h"
)

#定义源文件和头文件
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE FORMS "src/*.ui")
file(GLOB_RECURSE RESOURCES "data/*.qrc")
file(GLOB_RECURSE QSS "data/*.qss")


include_directories("${CMAKE_BINARY_DIR}" "src" "${INCLUDE_PATH}"
						 "${KIRAN_CONTROL_PANEL_PKG_INCLUDE_DIRS}" 
						 "${KLOG_QT5_INCLUDE_DIRS}" )

qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )


add_library(${TARGET_NAME} SHARED
	${SOURCES}
	${HEADERS}
	${FORMS}
	${RESOURCES}
	${QM_FILES}
	${QSS})
			
target_link_libraries(${TARGET_NAME}
			Qt5::Widgets
			Qt5::DBus
			Qt5::Svg
			Qt5::PrintSupport
			Qt5::Core
			qrencode
			glib-2.0
			kiranwidgets-qt5
			${KIRAN_WIDGETS_QT5_LIBRARIES}
			${KLOG_QT5_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CPANEL_PLUGIN_DIR}/)

install(FILES ${QM_FILES}
        DESTINATION ${TRANSLATIONS_DIR})

add_subdirectory(data)
add_subdirectory(config)

