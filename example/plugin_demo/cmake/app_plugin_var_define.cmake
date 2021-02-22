if(DEFINED PLUGIN_DIR) #此路径通过控制中心命令获取：/usr/bin/kiran-control-center --print-plugins-path。
	configure_file(${CMAKE_SOURCE_DIR}/cmake/plugin_config.h.in ${CMAKE_BINARY_DIR}/config.h)
    #ADD_DEFINITIONS(-DPLUGIN_SUB_ITME_ICON_DIR=\"${PLUGIN_SUB_ITME_ICON_DIR}/\")  #功能项图标
    #ADD_DEFINITIONS(-DPLUGIN_SUB_ITME_TRANSLATE_DIR_PREFIX=\"${PLUGIN_SUB_ITME_TRANSLATE_DIR}${PROJECT_NAME}\")  #功能项翻译前缀

else()
    file(GLOB APP_ICON_FILE "${CMAKE_SOURCE_DIR}/desktop/app/images/*")
    get_filename_component(APP_ICON_NAME ${APP_ICON_FILE} NAME)
	configure_file(${CMAKE_SOURCE_DIR}/cmake/app_config.h.in ${CMAKE_BINARY_DIR}/config.h)
	
    #ADD_DEFINITIONS(-DAPP_ICON_PATH=\"${APP_ICON_DIR}/${APP_ICON_NAME}\") #app图标

    #因为翻译文件有多个，不好通过cmake函数获取统一的文件名，所以这里手动定为工程名称。
    #ADD_DEFINITIONS(-DAPP_TRANSLATE_PATH_PREFIX=\"${APP_TRANSLATE_DIR}/${PROJECT_NAME}\") #app翻译前缀

endif()
