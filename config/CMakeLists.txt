
set(ICON_DIR "${DATA_DIR}/${PROJECT_NAME}/icons")

#添加kiran-cpanel-system.desktop.in文件
configure_file (
"${PROJECT_SOURCE_DIR}/config/kiran-cpanel-system.desktop.in"
"${PROJECT_BINARY_DIR}/config/kiran-cpanel-system.desktop"
)

install(FILES "${PROJECT_BINARY_DIR}/config/kiran-cpanel-system.desktop"
	DESTINATION ${CPANEL_DESKTOP_DIR}/ )




