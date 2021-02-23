#app
configure_file(${CMAKE_SOURCE_DIR}/plugin-single/desktop/app/files/${PROJECT_NAME}.desktop.in ${CMAKE_BINARY_DIR}/plugin-single/desktop/app/files/${PROJECT_NAME}.desktop)
#文件
file(GLOB desktop_files "${CMAKE_BINARY_DIR}/plugin-single/desktop/app/files/*")
install(FILES ${desktop_files} DESTINATION ${INSTALL_DATADIR}/applications)
####图片
file(GLOB desktop_images "app/images/*")
install(FILES ${desktop_images} DESTINATION ${SYSTEM_IMAGES_DIR})
