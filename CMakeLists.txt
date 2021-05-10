cmake_minimum_required(VERSION 3.0)

project(kiran-cpanel-mouse)

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

set(TRANSLATIONS_DIR "${DATA_DIR}/${PROJECT_NAME}/translations/")
set( TS_FILES "translations/kiran-cpanel-mouse.zh_CN.ts" )
set( INCLUDE_PATH "/usr/include/glib-2.0/" "/usr/lib64/glib-2.0/include/" "/usr/include" "/usr/include/kiranwidgets-qt5")

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
file(GLOB_RECURSE QSS "resources/themes/*.qss")


include_directories("${CMAKE_BINARY_DIR}" "src" "${INCLUDE_PATH}" )

find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools Network)

qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )


add_executable(kiran-cpanel-mouse 
			${SOURCES}
			${HEADERS}
			${FORMS}
			${RESOURCES}
	       ${QM_FILES}
			${QSS})
			
target_link_libraries(kiran-cpanel-mouse 
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
		 Qt5::Core
        kiranwidgets-qt5
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
		 ${ZLOG_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)

install(FILES ${QM_FILES}
        DESTINATION ${TRANSLATIONS_DIR})

add_subdirectory(config)
add_subdirectory(resources/images)

