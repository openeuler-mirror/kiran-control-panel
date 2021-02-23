#由于cmakelist向下继承变量，所以这里提前申明宏定义。
if(DEFINED PLUGIN_DIR) #此路径通过控制中心命令获取：/usr/bin/kiran-control-center --print-plugins-dir。
    #sub item 的图标和翻译
    set(PLUGIN_SUB_ITME_ICON_DIR       ${PLUGIN_DIR}/subitem-icons)
    set(PLUGIN_SUB_ITME_TRANSLATE_DIR  ${PLUGIN_DIR}/subitem-translations/)

else()
    #desktop icon / app icon
    set(APP_DESKTOP_DIR    ${INSTALL_DATADIR}/applications)
    set(APP_ICON_DIR       ${INSTALL_DATADIR}/${PROJECT_NAME}/icons/)
    #翻译
    set(APP_TRANSLATE_DIR  ${INSTALL_DATADIR}/${PROJECT_NAME}/translations/)
endif()




