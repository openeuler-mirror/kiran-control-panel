#set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}")

set(ICON_DIR "${DATA_DIR}/${PROJECT_NAME}/icons")
set(BIN_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")

#添加kiran-cpanel-keyboard.desktop.in文件
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/kiran-cpanel-keyboard.desktop.in
		${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-keyboard.desktop)


install(FILES "${PROJECT_BINARY_DIR}/config/kiran-cpanel-keyboard.desktop"
	DESTINATION ${CPANEL_DESKTOP_DIR}/ )

SET(link_source ${CPANEL_DESKTOP_DIR}/kiran-cpanel-keyboard.desktop)

add_custom_target(kiran-cpanel-keyboard-desktop ALL
	COMMAND ln -sf ${link_source} ${PROJECT_BINARY_DIR}/kiran-cpanel-keyboard.desktop)

install(FILES "${PROJECT_BINARY_DIR}/kiran-cpanel-keyboard.desktop"
	DESTINATION ${DATA_DIR}/applications/)

