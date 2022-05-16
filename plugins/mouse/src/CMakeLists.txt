#定义源文件和头文件
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS "*.ui")
file(GLOB_RECURSE RESOURCES "${PROJECT_SOURCE_DIR}/resources/*.qrc")

qt5_create_translation( QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} )

kiran_qt5_add_dbus_interface_ex(MOUSE_PROXY
	dbus-interface/com.kylinsec.Kiran.SessionDaemon.Mouse.xml
	mouse_backEnd_proxy
	MouseBackEndProxy
)

kiran_qt5_add_dbus_interface_ex(TOUCHPAD_PROXY
	dbus-interface/com.kylinsec.Kiran.SessionDaemon.TouchPad.xml
	touchPad_backEnd_proxy
	TouchPadBackEndProxy
)

add_library(${TARGET_NAME} SHARED
	${SOURCES}
	${HEADERS}
	${FORMS}
	${MOUSE_PROXY}
	${TOUCHPAD_PROXY}
	${RESOURCES}
	${QM_FILES})

target_include_directories(${TARGET_NAME} PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../
	${KCP_PLUGIN_INCLUDE_DIR}
	${KLOG_QT5_INCLUDE_DIRS}
	${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
	${KIRAN_CC_DAEMON_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
	Qt5::Widgets
	Qt5::DBus
	Qt5::Svg
	Qt5::Core
	${KIRANWIDGETS_QT5_LIBRARIES}
	${KLOG_QT5_LIBRARIES})

install(TARGETS ${TARGET_NAME}
        DESTINATION ${CPANEL_PLUGIN_DIR}/)

install(FILES ${QM_FILES}
        DESTINATION ${TRANSLATIONS_DIR})

