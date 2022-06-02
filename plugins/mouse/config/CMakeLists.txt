#set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}")

set(ICON_DIR "${DATA_DIR}/${PROJECT_NAME}/icons")
set(BIN_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")

#添加kiran-cpanel-mouse.desktop.in文件
configure_file (
${CMAKE_CURRENT_SOURCE_DIR}/kiran-cpanel-mouse.desktop.in
${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-mouse.desktop
)


install(FILES "${CMAKE_CURRENT_BINARY_DIR}/config/kiran-cpanel-mouse.desktop"
	DESTINATION ${PLUGIN_DESKTOP_INSTALL_DIR}/ )

SET(link_source ${PLUGIN_DESKTOP_INSTALL_DIR}/kiran-cpanel-mouse.desktop)

add_custom_target(kiran-cpanel-mouse-desktop ALL
	COMMAND ln -sf ${link_source} ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-mouse.desktop)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-mouse.desktop"
	DESTINATION ${DATA_DIR}/applications/)

