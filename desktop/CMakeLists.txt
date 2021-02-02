
if(DEFINED PLUGIN_DIR)
    #文件
    file(GLOB desktop_files "plugin/files/*")
    install(FILES ${desktop_files} DESTINATION ${KDT_DESKTOP_DIR})
    #图片
    file(GLOB desktop_images "plugin/images/*")
    set(KDT_DESKTOP_IMAGES_DIR ${KDT_DESKTOP_DIR}/${PROJECT_NAME}/icons)
    install(FILES ${desktop_images} DESTINATION ${KDT_DESKTOP_IMAGES_DIR})

else()
    #文件
    file(GLOB desktop_files "app/files/*")
    set(KDT_DESKTOP_DIR ${KDT_INSTALL_DATADIR}/applications)
    install(FILES ${desktop_files} DESTINATION ${KDT_DESKTOP_DIR})
    #图片
    file(GLOB desktop_images "app/images/*")
    install(FILES ${desktop_images} DESTINATION ${KDT_DESKTOP_IMAGES_DIR})

endif()
