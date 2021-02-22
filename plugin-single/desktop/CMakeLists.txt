
#文件
file(GLOB desktop_files "app/files/*")
set(KCP_DESKTOP_DIR ${KCP_INSTALL_DATADIR}/applications)
install(FILES ${desktop_files} DESTINATION ${KCP_DESKTOP_DIR})
#图片
file(GLOB desktop_images "app/images/*")
install(FILES ${desktop_images} DESTINATION ${KCP_DESKTOP_IMAGES_DIR})
