set(ICON_DIR "${DATA_DIR}/${TARGET_NAME}/icons")


configure_file (
"${CMAKE_CURRENT_SOURCE_DIR}/kiran-cpanel-appearance.desktop.in"
"${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-appearance.desktop"
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-appearance.desktop"
        DESTINATION ${CPANEL_DESKTOP_DIR}/ )

SET(source ${PLUGIN_DESKTOP_INSTALL_DIR}/kiran-cpanel-appearance.desktop)

add_custom_target(kiran-cpanel-appearance-desktop ALL
	          COMMAND ln -sf ${source} ${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-appearance.desktop)


install(FILES "${CMAKE_CURRENT_BINARY_DIR}/kiran-cpanel-appearance.desktop"
	DESTINATION ${DATA_DIR}/applications/)

