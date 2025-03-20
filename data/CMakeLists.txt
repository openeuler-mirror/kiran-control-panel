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