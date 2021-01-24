

cmake_minimum_required(VERSION 3.5)

project(kiran-display-tools)
find_package(PkgConfig REQUIRED)
#读取pc文件
pkg_search_module(KIRANWIDGETS_QT5 REQUIRED kiranwidgets-qt5)
include_directories(${KIRANWIDGETS_QT5_INCLUDE_DIRS})
link_directories(${KIRANWIDGETS_QT5_LIBRARY_DIRS})

pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
include_directories(${KIRAN_CC_DAEMON_INCLUDE_DIRS})

pkg_search_module(KIRAN_CONTROL_PANEL REQUIRED kiran-control-panel)
include_directories(${KIRAN_CONTROL_PANEL_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE kiran-combobox "kiran-combobox.*")
file(GLOB_RECURSE kiran-display-config-global "kiran-display-config-global.*")
file(GLOB_RECURSE kiran-display-config-identifying "kiran-display-config-identifying.*")
file(GLOB_RECURSE kiran-display-config-item "kiran-display-config-item.*")
file(GLOB_RECURSE kiran-display-config-item-contain "kiran-display-config-item-contain.*")
file(GLOB_RECURSE kiran-display-configuration "kiran-display-configuration.*")
file(GLOB_RECURSE kiran-display-configuration-panel "kiran-display-configuration-panel.*")
file(GLOB_RECURSE kiran-display-configuration-window "kiran-display-configuration-window.*")
file(GLOB_RECURSE kiran-display-module-base "kiran-display-module-base.*")

file(GLOB_RECURSE RESOURCES "./*.qrc")
file(GLOB_RECURSE TS_FILES "./*.ts")

find_package(Qt5 COMPONENTS Widgets DBus X11Extras Svg LinguistTools)
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
#编译参数传入
include(GNUInstallDirs)
set(KDT_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
set(KDT_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(KDT_INSTALL_DATADIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR})

if(${BUILD_APP})
    file(GLOB_RECURSE main "main.cpp")
    file(GLOB_RECURSE desktop_files "app-desktop/files/*")
    file(GLOB_RECURSE desktop_images "app-desktop/images/*")
    add_executable(${PROJECT_NAME}
        ${kiran-combobox}
        ${kiran-display-config-global}
        ${kiran-display-config-identifying}
        ${kiran-display-config-item}
        ${kiran-display-config-item-contain}
        ${kiran-display-configuration}
        ${kiran-display-configuration-panel}
        ${kiran-display-configuration-window}
        ${kiran-display-module-base}
        ${RESOURCES}
        ${QM_FILES}
        ${main})

    #翻译
    SET(TRANSLATE_PATH ${KDT_INSTALL_DATADIR}/${PROJECT_NAME}/translations/)
    install(FILES ${QM_FILES} DESTINATION ${TRANSLATE_PATH})
    ADD_DEFINITIONS(-DTRANSLATE_PREFIX=\"${TRANSLATE_PATH}${PROJECT_NAME}\")
    #desktop and desktop image
    set(KDT_DESKTOP_DIR ${KDT_INSTALL_DATADIR}/applications)
    install(FILES ${desktop_files} DESTINATION ${KDT_DESKTOP_DIR})
    set(KDT_DESKTOP_IMAGES_DIR ${KDT_INSTALL_DATADIR}/icons/Kiran/places/16x16/)
    install(FILES ${desktop_images} DESTINATION ${KDT_DESKTOP_IMAGES_DIR})

    #安装
    install(TARGETS ${PROJECT_NAME} DESTINATION ${KDT_INSTALL_BINDIR}/)
else()
    file(GLOB_RECURSE interface "interface.*")
    file(GLOB_RECURSE desktop_files "desktop/files/*")
    file(GLOB_RECURSE desktop_images "desktop/images/*")
    add_library(${PROJECT_NAME} SHARED
        ${kiran-combobox}
        ${kiran-display-config-global}
        ${kiran-display-config-identifying}
        ${kiran-display-config-item}
        ${kiran-display-config-item-contain}
        ${kiran-display-configuration}
        ${kiran-display-configuration-panel}
        ${kiran-display-configuration-window}
        ${kiran-display-module-base}
        ${RESOURCES}
        ${QM_FILES}
        ${interface})
    #desktop and desktop image
    set(KDT_DESKTOP_DIR ${KDT_INSTALL_DATADIR}/kiran-control-panel/plugins)
    install(FILES ${desktop_files} DESTINATION ${KDT_DESKTOP_DIR})
    set(KDT_DESKTOP_IMAGES_DIR ${KDT_DESKTOP_DIR}/${PROJECT_NAME}/icons)
    install(FILES ${desktop_images} DESTINATION ${KDT_DESKTOP_IMAGES_DIR})
    #翻译
    SET(PLUGIN_PATH ${KDT_DESKTOP_DIR}/${PROJECT_NAME})
    SET(PLUGIN_TRANSLATE_PATH ${PLUGIN_PATH}/translate/)
    install(FILES ${QM_FILES} DESTINATION ${PLUGIN_TRANSLATE_PATH})
    ADD_DEFINITIONS(-DPLUGIN_TRANSLATE_PATH_PREFIX=\"${PLUGIN_TRANSLATE_PATH}${PROJECT_NAME}\")
    ADD_DEFINITIONS(-DPLUGIN_TRANSLATE_ICON_PATH=\"${PLUGIN_PATH}/icons/\")
    #安装
    install(TARGETS ${PROJECT_NAME} DESTINATION ${KDT_INSTALL_LIBDIR}/)
endif()

target_link_libraries(${PROJECT_NAME}
    Qt5::Core Qt5::Gui Qt5::Svg Qt5::Widgets Qt5::X11Extras Qt5::DBus kiranwidgets-qt5 X11 ${KIRANWIDGETS_QT5_LIBRARIES})






