#app
configure_file(${CMAKE_SOURCE_DIR}/plugin-group/desktop/app/files/${PROJECT_NAME}.desktop.in ${CMAKE_BINARY_DIR}/plugin-group/desktop/app/files/${PROJECT_NAME}.desktop)
#文件
file(GLOB desktop_files "${CMAKE_BINARY_DIR}/plugin-group/desktop/app/files/*")
install(FILES ${desktop_files} DESTINATION ${INSTALL_DATADIR}/applications)
####图片
file(GLOB desktop_images "app/images/*")
install(FILES ${desktop_images} DESTINATION ${SYSTEM_IMAGES_DIR})

#category
####文件
file(GLOB desktop_files "category/files/*")
install(FILES ${desktop_files} DESTINATION ${CATEGORY_DESKTOP_DIR})
####图片
file(GLOB desktop_images "category/images/*")
install(FILES ${desktop_images} DESTINATION ${CATEGORY_DESKTOP_ICON_DIR})
