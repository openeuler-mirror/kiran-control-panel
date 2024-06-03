include(GNUInstallDirs)

if(NOT CMAKE_INSTALL_FULL_BINDDIR)
    set(CMAKE_INSTALL_FULL_BINDDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
endif()

if(NOT CMAKE_INSTALL_FULL_DATADIR)
    set(CMAKE_INSTALL_FULL_DATADIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
endif()

if(NOT CMAKE_INSTALL_FULL_LIBEXECDIR)
    set(CMAKE_INSTALL_FULL_LIBEXECDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBEXECDIR}")
endif()

if(NOT CMAKE_INSTALL_FULL_INCLUDEDIR)
    set(CMAKE_INSTALL_FULL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
endif()

set(SYSCONFDIR "/etc" CACHE PATH "Installation directory for configurations")

set(KCP_DATADIR ${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME})
set(KCP_INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR}/${PROJECT_NAME})
set(KCP_SETTINGS ${KCP_DATADIR}/settings.ini)

set(CATEGORY_DIR ${KCP_DATADIR}/category)
set(CATEGORY_DESKTOP_DIR ${CATEGORY_DIR}/desktop)
set(CATEGORY_ICON_DIR ${CATEGORY_DIR}/icons)

set(PLUGIN_DIR ${KCP_DATADIR}/plugins)
set(PLUGIN_LIBS_DIR ${PLUGIN_DIR}/libs )
set(PLUGIN_DESKTOP_DIR ${PLUGIN_DIR}/desktop)

SET(TRANSLATION_DIR ${KCP_DATADIR}/translations/)

#账户插件
set(KCP_ACCOUNT_DATADIR ${KCP_DATADIR}/account)
set(ACCOUNT_BUILDIN_AVATAR_DIR ${KCP_ACCOUNT_DATADIR}/avatars)
set(ACCOUNT_DEFAULT_AVATAR ${ACCOUNT_BUILDIN_AVATAR_DIR}/0.face)
