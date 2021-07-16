
set(ICON_DIR "${DATA_DIR}/${PROJECT_NAME}/icons")

#添加kiran-cpanel-system.desktop.in文件
configure_file (
"${PROJECT_SOURCE_DIR}/config/kiran-cpanel-system.desktop.in"
"${PROJECT_BINARY_DIR}/config/kiran-cpanel-system.desktop"
)

install(FILES "${PROJECT_BINARY_DIR}/config/kiran-cpanel-system.desktop"
        DESTINATION ${CPANEL_DESKTOP_DIR}/ )

SET(source ${CPANEL_DESKTOP_DIR}/kiran-cpanel-system.desktop)

add_custom_target(kiran-cpanel-system-desktop ALL
	          COMMAND ln -sf ${source} ${PROJECT_BINARY_DIR}/kiran-cpanel-system.desktop)


install(FILES "${PROJECT_BINARY_DIR}/kiran-cpanel-system.desktop"
	DESTINATION ${DATA_DIR}/applications/)

