if(DEFINED PLUGIN_DIR)
	configure_file(${CMAKE_SOURCE_DIR}/desktop/plugin/files/${PROJECT_NAME}.desktop.in ${CMAKE_BINARY_DIR}/desktop/plugin/files/${PROJECT_NAME}.desktop)
    #文件
    file(GLOB desktop_files "${CMAKE_BINARY_DIR}/desktop/plugin/files/*")
    install(FILES ${desktop_files} DESTINATION ${PLUGIN_DIR})
    #图片
    file(GLOB desktop_images "plugin/images/*")
    install(FILES ${desktop_images} DESTINATION ${PLUGIN_DIR}/icons)
    #功能项图片
    file(GLOB desktop_item_images "plugin/subitem-images/*")
    install(FILES ${desktop_item_images} DESTINATION ${PLUGIN_SUB_ITME_ICON_DIR})

else()
	configure_file(${CMAKE_SOURCE_DIR}/desktop/app/files/${PROJECT_NAME}.desktop.in ${CMAKE_BINARY_DIR}/desktop/app/files/${PROJECT_NAME}.desktop)
    #文件
    file(GLOB desktop_files "${CMAKE_BINARY_DIR}/desktop/app/files/*")
    install(FILES ${desktop_files} DESTINATION ${APP_DESKTOP_DIR})
    #图片
    file(GLOB desktop_images "app/images/*")
    install(FILES ${desktop_images} DESTINATION ${APP_ICON_DIR})

endif()
