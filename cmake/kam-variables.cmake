include(GNUInstallDirs)

#账户管理数据目录
set(KAM_DATA_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME} )
#账户管理内置头像安装目录
set(KAM_BUILDIN_AVATAR_DIR_PATH ${KAM_DATA_DIR}/account-icons )
#账户管理Desktop文件图标和系统标题栏图标路径
set(KAM_DESKTOP_ICON_PATH       ${KAM_DATA_DIR}/account-management.png )
#账户管理翻译目录
set(KAM_TRANSLATION_DIR_PATH    ${KAM_DATA_DIR}/translations )

#账户管理Desktop安装路径
set(KAM_DESKTOP_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/applications )

#账户管理安装路径
set(KAM_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR} )

#-------------------------------------------

set(KAM_AVATAR_EDITOR_DIR      ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBEXECDIR} )
set(KAM_AVATAR_EDITOR_PATH     ${KAM_AVATAR_EDITOR_DIR}/kiran-avatar-editor )