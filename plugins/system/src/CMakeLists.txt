if(${CMAKE_VERSION} VERSION_LESS 3.0)
    file(GLOB_RECURSE QT5_UI_FILES ${PROJECT_SOURCE_DIR}/src/*.ui)
    qt5_wrap_ui(FORMS ${QT5_UI_FILES})
    add_custom_target(uic_file ALL DEPENDS ${FORMS})

    file(GLOB_RECURSE RESOURCES_FILES "${PROJECT_SOURCE_DIR}/data/*.qrc")
    qt5_add_resources(RESOURCES ${RESOURCES_FILES})

else()
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    file(GLOB_RECURSE RESOURCES "${PROJECT_SOURCE_DIR}/data/*.qrc")
    file(GLOB_RECURSE FORMS "*.ui")

endif()

#定义源文件和头文件
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

#授权源码
file(GLOB_RECURSE LICENSE_CPP "pages/license-information/*.cpp" "license/*.cpp" "dbus-wrapper/license-dbus.cpp" )
file(GLOB_RECURSE LICENSE_H "pages/license-information/*.h" "license/*.h"  "dbus-wrapper/license-dbus.h")
file(GLOB_RECURSE LICENSE_FORMS "pages/license-information/*.ui" "license/*.ui")

#系统信息源码
file(GLOB_RECURSE SYSTEM_INFO_CPP "pages/hardware-information/*.cpp" "pages/system-information/*.cpp" "dbus-wrapper/system-info-dbus.cpp" )
file(GLOB_RECURSE SYSTEM_INFO_H "pages/hardware-information/*.h" "pages/system-information/*.h"  "dbus-wrapper/system-info-dbus.h")
file(GLOB_RECURSE SYSTEM_INFO_FORMS "pages/hardware-information/*.ui" "pages/system-information/*.ui")

#公共源码
file(GLOB_RECURSE PUBLIC_CPP "custom-widgets/*.cpp" "kiran-system-information.cpp" "main.cpp")
file(GLOB_RECURSE PUBLIC_H "custom-widgets/*.h" "kiran-system-information.h")
file(GLOB_RECURSE PUBLIC_FORMS "custom-widgets/*.ui" "kiran-system-information.ui")

#插件接口源码
file(GLOB_RECURSE PLUGIN_INTERFACE_CPP "plug_interface/*.cpp")
file(GLOB_RECURSE PLUGIN_INTERFACE_H "plug_interface/*.h")

file(GLOB_RECURSE QSS "${PROJECT_SOURCE_DIR}/data/*.qss")

qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )

if(DISABLE_SYSTEM_INFO)
        add_executable(${PROJECT_NAME}
                ${LICENSE_CPP}
                ${LICENSE_H}
                ${LICENSE_FORMS}
                ${PUBLIC_CPP}
                ${PUBLIC_H}
                ${PUBLIC_FORMS}
                ${FORMS}
                ${RESOURCES}
                ${QSS}
                ${QM_FILES})

        if(DISABLE_KIRANWIDGETS)
                target_include_directories(${PROJECT_NAME} PRIVATE
                        ${CMAKE_BINARY_DIR}
                        "/usr/include/ks-license"
                        ${KLOG_QT5_INCLUDE_DIRS})

                target_link_libraries(${PROJECT_NAME}
                        Qt5::Widgets
                        Qt5::DBus
                        Qt5::Svg
                        Qt5::Core
                        Qt5::PrintSupport
                        qrencode
                        glib-2.0
                        ${KLOG_QT5_LIBRARIES})
        else()

                target_include_directories(${PROJECT_NAME} PRIVATE
                        ${CMAKE_BINARY_DIR}
                        "/usr/include/ks-license"
                        ${KLOG_QT5_INCLUDE_DIRS}
                        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS})

		target_link_libraries(${PROJECT_NAME}
			Qt5::Widgets
			Qt5::DBus
			Qt5::Svg
			Qt5::Core
			Qt5::PrintSupport
			qrencode
			glib-2.0
                        ${KLOG_QT5_LIBRARIES}
                        ${KIRAN_WIDGETS_QT5_LIBRARIES})
       endif()

	install(TARGETS ${PROJECT_NAME}
		DESTINATION ${BIN_DIR}/)

else()
	add_library(${PROJECT_NAME} SHARED
        ${SOURCES}
        ${HEADERS}
        ${FORMS}
        ${RESOURCES}
        ${QM_FILES}
        ${QSS})

	target_include_directories(${PROJECT_NAME} PRIVATE
		${CMAKE_BINARY_DIR}
                "/usr/include/ks-license"
		${KIRAN_CONTROL_PANEL_PKG_INCLUDE_DIRS}
		${KLOG_QT5_INCLUDE_DIRS}
		${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
		${KIRAN_CC_DAEMON_INCLUDE_DIRS})
	
	target_link_libraries(${PROJECT_NAME}
		Qt5::Widgets
		Qt5::DBus
		Qt5::Svg
		Qt5::Core
		Qt5::PrintSupport
		qrencode
		glib-2.0
                ${KIRAN_WIDGETS_QT5_LIBRARIES}
		${KLOG_QT5_LIBRARIES})

	install(TARGETS ${PROJECT_NAME}
		DESTINATION ${CPANEL_PLUGIN_DIR}/)

endif()


install(FILES ${QM_FILES}
	DESTINATION ${TRANSLATIONS_DIR} RENAME ${PROJECT_NAME}.zh_CN.qm)

