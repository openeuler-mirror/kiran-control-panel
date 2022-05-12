set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE APPEARANCE_DBUS_XML "dbus-interface/com.kylinsec.Kiran.SessionDaemon.Appearance.xml")

#定义源文件和头文件
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS "*.ui")
file(GLOB_RECURSE RESOURCES "${PROJECT_SOURCE_DIR}/resources/*.qrc")
file(GLOB_RECURSE QSS "${PROJECT_SOURCE_DIR}/resources/themes/*.qss")


qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )

set_source_files_properties(${APPEARANCE_DBUS_XML} PROPERTIES
	CLASSNAME "AppearanceBackEndProxy"
	NO_NAMESPACE true
)

kiran_qt5_add_dbus_interface(APPEARANCE_PROXY ${APPEARANCE_DBUS_XML} appearance_backEnd_proxy)

add_library(${PROJECT_NAME} SHARED
	${SOURCES}
	${HEADERS}
	${FORMS}
	${APPEARANCE_PROXY}
	${RESOURCES}
	${QM_FILES}
	${QSS})

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_BINARY_DIR}
        ${KIRAN_CONTROL_PANEL_PKG_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} 
	Qt5::Widgets
	Qt5::DBus
	Qt5::Svg
	Qt5::Core
	Xcursor
	X11
	${KIRANWIDGETS_QT5_LIBRARIES}
	${KLOG_QT5_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CPANEL_PLUGIN_DIR}/)

install(FILES ${QM_FILES}
        DESTINATION ${TRANSLATIONS_DIR})

