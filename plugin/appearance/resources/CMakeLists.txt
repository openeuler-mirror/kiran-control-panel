set(KCP_ICON_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor)

install(FILES "images/kcp-appearance-24.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/24x24/apps RENAME kcp-appearance.png)
install(FILES "images/kcp-appearance-32.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/32x32/apps RENAME kcp-appearance.png)
install(FILES "images/kcp-appearance-48.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/48x48/apps RENAME kcp-appearance.png)
install(FILES "images/kcp-appearance-64.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/64x64/apps RENAME kcp-appearance.png)
install(FILES "images/kcp-appearance-96.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/96x96/apps RENAME kcp-appearance.png)

install(FILES "images/kcp-page-wallpaper.svg"
	DESTINATION ${DATA_DIR}/${PROJECT_NAME}/icons/ )

install(FILES "images/kcp-page-themes.svg"
	DESTINATION ${DATA_DIR}/${PROJECT_NAME}/icons/ )

install(FILES "images/kcp-page-font.svg"
	DESTINATION ${DATA_DIR}/${PROJECT_NAME}/icons/ )

