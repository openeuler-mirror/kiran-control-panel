#set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}")

set(ICON_DIR "${DATA_DIR}/${PROJECT_NAME}/icons")
set(BIN_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")

#添加kiran-cpanel-keybinding.desktop.in文件
configure_file (
${CMAKE_CURRENT_SOURCE_DIR}/kiran-cpanel-keybinding.desktop.in
${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-keybinding.desktop
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/config/kiran-cpanel-keybinding.desktop"
	DESTINATION ${PLUGIN_DESKTOP_INSTALL_DIR}/ )

SET(link_source ${PLUGIN_DESKTOP_INSTALL_DIR}/kiran-cpanel-keybinding.desktop)

add_custom_target(kiran-cpanel-keybinding-desktop ALL
	COMMAND ln -sf ${link_source} ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-keybinding.desktop)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-keybinding.desktop"
	DESTINATION ${DATA_DIR}/applications/)

