#set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}")

set(ICON_DIR "${DATA_DIR}/${PROJECT_NAME}/icons")
set(BIN_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")

#添加kiran-cpanel-keybinding.desktop.in文件
configure_file (
"${PROJECT_SOURCE_DIR}/config/kiran-cpanel-keybinding.desktop.in"
"${PROJECT_BINARY_DIR}/config/kiran-cpanel-keybinding.desktop"
)


install(FILES "${PROJECT_BINARY_DIR}/config/kiran-cpanel-keybinding.desktop"
	DESTINATION ${CPANEL_DESKTOP_DIR}/ )

SET(link_source ${CPANEL_DESKTOP_DIR}/kiran-cpanel-keybinding.desktop)

add_custom_target(kiran-cpanel-keybinding-desktop ALL
	COMMAND ln -sf ${link_source} ${PROJECT_BINARY_DIR}/kiran-cpanel-keybinding.desktop)

install(FILES "${PROJECT_BINARY_DIR}/kiran-cpanel-keybinding.desktop"
	DESTINATION ${DATA_DIR}/applications/)

