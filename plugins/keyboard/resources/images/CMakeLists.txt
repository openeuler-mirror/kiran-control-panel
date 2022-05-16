set(KCP_ICON_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor)

install(FILES "kcp-keyboard-24.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/24x24/apps RENAME kcp-keyboard.png )
install(FILES "kcp-keyboard-32.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/32x32/apps RENAME kcp-keyboard.png )
install(FILES "kcp-keyboard-48.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/48x48/apps RENAME kcp-keyboard.png )
install(FILES "kcp-keyboard-64.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/64x64/apps RENAME kcp-keyboard.png )
install(FILES "kcp-keyboard-96.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/96x96/apps RENAME kcp-keyboard.png )

install(FILES "kck-page-generation.svg"
	DESTINATION ${DATA_DIR}/${PROJECT_NAME}/icons/ )

install(FILES "kck-page-layout.png"
	DESTINATION ${DATA_DIR}/${PROJECT_NAME}/icons/ )
