set(ICON_DIR "${SHARE_INSTALL_PREFIX}/${PROJECT_NAME}/icons")
set(BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin")

#添加kiran-system-information.desktop.in文件
configure_file (
"${PROJECT_SOURCE_DIR}/config/kiran-system-information.desktop.in"
"${PROJECT_BINARY_DIR}/config/kiran-system-information.desktop"
)


install(FILES "${PROJECT_BINARY_DIR}/config/kiran-system-information.desktop"
        DESTINATION ${SHARE_INSTALL_PREFIX}/applications/ )




