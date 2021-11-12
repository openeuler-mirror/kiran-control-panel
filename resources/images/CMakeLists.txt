set(KCP_ICON_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor)

install(FILES "kcp-mouse-24.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/24x24/apps RENAME kcp-mouse.png)
install(FILES "kcp-mouse-32.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/32x32/apps RENAME kcp-mouse.png)
install(FILES "kcp-mouse-48.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/48x48/apps RENAME kcp-mouse.png)
install(FILES "kcp-mouse-64.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/64x64/apps RENAME kcp-mouse.png)
install(FILES "kcp-mouse-96.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/96x96/apps RENAME kcp-mouse.png)

install(FILES "kcm-page-mouse.svg"
	DESTINATION ${DATA_DIR}/${PROJECT_NAME}/icons/ )

install(FILES "kcm-page-touchpad.svg"
	DESTINATION ${DATA_DIR}/${PROJECT_NAME}/icons/ )
