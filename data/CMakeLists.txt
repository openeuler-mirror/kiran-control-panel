# 主题图标
install(DIRECTORY hicolor DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/icons/)

# 安装主分类文件
file(GLOB CATEGORY_DESKTOP "category/desktop/*.desktop")
if(NOT ENABLE_NETWORK)
    list(FILTER CATEGORY_DESKTOP EXCLUDE REGEX "network.desktop")
endif()

if(NOT ENABLE_AUDIO)
    list(FILTER CATEGORY_DESKTOP EXCLUDE REGEX "audio.desktop")
endif()

if(NOT ENABLE_USER_GROUP)
    list(FILTER CATEGORY_DESKTOP EXCLUDE REGEX "group.desktop")
endif()

if(NOT ENABLE_APPLICATION)
    list(FILTER CATEGORY_DESKTOP EXCLUDE REGEX "application.desktop")
endif()

install(FILES ${CATEGORY_DESKTOP} DESTINATION ${CATEGORY_DESKTOP_DIR})

# 安装主分类相关图标文件
file(GLOB CATEGORY_ICONS "./category/images/*")
install(FILES ${CATEGORY_ICONS} DESTINATION ${CATEGORY_ICON_DIR})

# 主面板desktop
configure_file(${CMAKE_SOURCE_DIR}/data/${PROJECT_NAME}.desktop.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.desktop DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/applications/)

# 控制中心pkgconfig文件
configure_file(${CMAKE_SOURCE_DIR}/data/${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

install(DIRECTORY ./avatars DESTINATION ${KCP_ACCOUNT_DATADIR} )

# 声音测试音频文件
file(GLOB SOUND_FILES "sounds/*.ogg")
install(FILES ${SOUND_FILES} DESTINATION ${AUDIO_SOUNDS_DIR})

# GSettings 配置
#
# 安装的 gschema 由顶层 CMakeLists 依据构建参数 SERVER_MODE 生成，server-mode 的默认值
# 在构建期确定(服务器版 true / 桌面版 false)，仅为兼容性考虑：让各形态的包自带正确的
# 默认值，升级控制中心时不必依赖 kiran-desktop 等其它包同步升级。
#
# 该默认值可被更高优先级的配置覆写，定制化配置推荐放到 kiran-desktop 的 override 机制：
#   1. kiran-desktop 的 51_kiran-server.gschema.override(可维护、可 override)
#   2. dconf system-db(/etc/dconf/db/*.d，镜像/现场定制)
#   3. 用户 dconf(gsettings set)
install(FILES ${CMAKE_BINARY_DIR}/com.kylinsec.kiran.control-panel.plugin.gschema.xml
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/glib-2.0/schemas/)