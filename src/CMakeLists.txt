set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE KEYBOARD_DBUS_XML "dbus-wrapper/com.kylinsec.Kiran.SessionDaemon.Keyboard.xml")

#定义源文件和头文件
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS "*.ui")
file(GLOB_RECURSE RESOURCES "${PROJECT_SOURCE_DIR}/resources/*.qrc")

qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )

set_source_files_properties(${KEYBOARD_DBUS_XML} PROPERTIES
	CLASSNAME "KeyboardBackEndProxy"
	NO_NAMESPACE true
)

kiran_qt5_add_dbus_interface(KEYBOARD_PROXY ${KEYBOARD_DBUS_XML} keyboard_backEnd_proxy)

add_library(${PROJECT_NAME} SHARED
	${SOURCES}
	${HEADERS}
	${FORMS}
	${KEYBOARD_PROXY}
	${RESOURCES}
	${QM_FILES})

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
	${KIRANWIDGETS_QT5_LIBRARIES}
	${KLOG_QT5_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CPANEL_PLUGIN_DIR}/)

install(FILES ${QM_FILES}
        DESTINATION ${TRANSLATIONS_DIR})

