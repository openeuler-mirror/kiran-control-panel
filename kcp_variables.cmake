include(GNUInstallDirs)

set(INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
set(INSTALL_DATADIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR})
set(INSTALL_LIBEXEC ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBEXECDIR})

set(KCP_DATADIR ${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME})
set(KCP_INSTALL_INCLUDE ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

set(SYSCONFDIR "/etc" CACHE PATH "Installation directory for configurations")

set(KCP_PLUGIN_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(KCP_SETTINGS ${KCP_DATADIR}/settings.ini)

#根级分类安装路径
set(CATEGORY_INSTALL_DIR ${INSTALL_DATADIR}/${PROJECT_NAME}/category)
##根级分类desktop安装路径
set(CATEGORY_DESKTOP_INSTALL_DIR ${CATEGORY_INSTALL_DIR}/desktop)
##根级分类icon安装路径
set(CATEGORY_ICON_INSTALL_DIR ${CATEGORY_INSTALL_DIR}/icons)

#插件so安装路径
set(PLUGIN_LIBS_INSTALL_DIR ${INSTALL_DATADIR}/${PROJECT_NAME}/plugins/libs )
#插件desktop安装路径
set(PLUGIN_DESKTOP_INSTALL_DIR ${INSTALL_DATADIR}/${PROJECT_NAME}/plugins/desktop)
#翻译文件安装路径
SET(TRANSLATION_INSTALL_DIR ${INSTALL_DATADIR}/${PROJECT_NAME}/translations/)