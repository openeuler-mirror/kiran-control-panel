cmake_minimum_required(VERSION 3.0)

project(kiran-cpanel-system)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PkgConfig REQUIRED)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(ZLOG REQUIRED zlog)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}")

set(TARGET_NAME ${PROJECT_NAME})
set(TRANSLATIONS_DIR "${DATA_DIR}/${PROJECT_NAME}/translations/")
set( TS_FILES "translations/kiran-cpanel-system.zh_CN.ts" )
set( INCLUDE_PATH "/usr/include/glib-2.0/" "/usr/lib64/glib-2.0/include/" "/usr/include" "/usr/include/kylin-license" "/usr/include/kiran-cc-daemon")

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


include_directories("${CMAKE_BINARY_DIR}" "src" "${INCLUDE_PATH}" )

find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools Network PrintSupport)

qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )


add_executable(${TARGET_NAME}
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
	QZint
	glib-2.0
        kiranwidgets-qt5
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
	${ZLOG_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)

install(FILES ${QM_FILES}
        DESTINATION ${TRANSLATIONS_DIR})

add_subdirectory(data)
add_subdirectory(config)

