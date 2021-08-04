
set(ICON_DIR "${DATA_DIR}/${PROJECT_NAME}/icons")

#添加kiran-cpanel-appearance.desktop.in文件
configure_file (
"${PROJECT_SOURCE_DIR}/config/kiran-cpanel-appearance.desktop.in"
"${PROJECT_BINARY_DIR}/config/kiran-cpanel-appearance.desktop"
)

install(FILES "${PROJECT_BINARY_DIR}/config/kiran-cpanel-appearance.desktop"
        DESTINATION ${CPANEL_DESKTOP_DIR}/ )

SET(source ${CPANEL_DESKTOP_DIR}/kiran-cpanel-appearance.desktop)

add_custom_target(kiran-cpanel-appearance-desktop ALL
	          COMMAND ln -sf ${source} ${PROJECT_BINARY_DIR}/kiran-cpanel-appearance.desktop)


install(FILES "${PROJECT_BINARY_DIR}/kiran-cpanel-appearance.desktop"
	DESTINATION ${DATA_DIR}/applications/)

