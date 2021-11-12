set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE MOUSE_DBUS "dbus-interface/com.kylinsec.Kiran.SessionDaemon.Mouse.xml")
file(GLOB_RECURSE TOUCHPAD_DBUS "dbus-interface/com.kylinsec.Kiran.SessionDaemon.TouchPad.xml")

#定义源文件和头文件
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS "*.ui")
file(GLOB_RECURSE RESOURCES "${PROJECT_SOURCE_DIR}/resources/*.qrc")

qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )

set_source_files_properties(${MOUSE_DBUS} PROPERTIES
	CLASSNAME "MouseBackEndProxy"
	NO_NAMESPACE true
)
set_source_files_properties(${TOUCHPAD_DBUS} PROPERTIES
	CLASSNAME "TouchPadBackEndProxy"
	NO_NAMESPACE true
)

kiran_qt5_add_dbus_interface(MOUSE_PROXY
	${MOUSE_DBUS}
	mouse_backEnd_proxy
)

kiran_qt5_add_dbus_interface(TOUCHPAD_PROXY
	${TOUCHPAD_DBUS}
	touchPad_backEnd_proxy
)

add_library(${PROJECT_NAME} SHARED
	${SOURCES}
	${HEADERS}
	${FORMS}
	${MOUSE_PROXY}
	${TOUCHPAD_PROXY}
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

