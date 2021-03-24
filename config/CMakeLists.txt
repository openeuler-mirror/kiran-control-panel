#set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}")

set(ICON_DIR "${DATA_DIR}/${PROJECT_NAME}/icons")
set(BIN_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")

#添加kiran-system-information.desktop.in文件
configure_file (
"${PROJECT_SOURCE_DIR}/config/kiran-system-information.desktop.in"
"${PROJECT_BINARY_DIR}/config/kiran-system-information.desktop"
)


install(FILES "${PROJECT_BINARY_DIR}/config/kiran-system-information.desktop"
	DESTINATION ${DATA_DIR}/applications/ )




