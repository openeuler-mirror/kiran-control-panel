set(KCP_ICON_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor)

install(FILES "icons/kcp-system-24.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/24x24/apps RENAME kcp-system.png)
install(FILES "icons/kcp-system-32.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/32x32/apps RENAME kcp-system.png)
install(FILES "icons/kcp-system-48.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/48x48/apps RENAME kcp-system.png)
install(FILES "icons/kcp-system-64.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/64x64/apps RENAME kcp-system.png)
install(FILES "icons/kcp-system-96.png"
	DESTINATION ${KCP_ICON_INSTALL_PATH}/96x96/apps RENAME kcp-system.png)

install(FILES "hardware-information.svg"
	DESTINATION ${DATA_DIR}/${PROJECT_NAME}/icons/ )

install(FILES "system-information.svg"
	DESTINATION ${DATA_DIR}/${PROJECT_NAME}/icons/ )

install(FILES "license-information.svg"
	DESTINATION ${DATA_DIR}/${PROJECT_NAME}/icons/ )
