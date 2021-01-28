cmake_minimum_required(VERSION 3.5)

project(kiran-control-center)
set(KIRAN_CONTROL_PANEL_VERSION     1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#公共部分代码
file(GLOB_RECURSE GLOBAL_C "../global/*.cpp")
file(GLOB_RECURSE GLOBAL_H "../global/*.h")
file(GLOB_RECURSE INTERFACE_C "../interface/*.cpp")
file(GLOB_RECURSE INTERFACE_H "../interface/*.h")
file(GLOB_RECURSE MODULE_WIDGET_C "../widgets/kiran-module-widget/*.cpp")
file(GLOB_RECURSE MODULE_WIDGET_H "../widgets/kiran-module-widget/*.h")
file(GLOB_RECURSE SEARCH_LINEEDIT_C "../widgets/kiran-search-lineedit/*.cpp")
file(GLOB_RECURSE SEARCH_LINEEDIT_H "../widgets/kiran-search-lineedit/*.h")
file(GLOB_RECURSE MAIN_C "*.cpp")
file(GLOB_RECURSE MAIN_H "*.h")
file(GLOB_RECURSE RESOURCES "./*.qrc")
file(GLOB_RECURSE TS_FILES "./*.ts")

find_package(PkgConfig REQUIRED)
pkg_search_module(KIRANWIDGETS_QT5 REQUIRED kiranwidgets-qt5)
find_package(Qt5 COMPONENTS Widgets DBus X11Extras Svg Xml LinguistTools)
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

qt5_wrap_ui(MODULE_WIDGET_UI "../widgets/kiran-module-widget/kiran-module-widget.ui")
add_executable(${PROJECT_NAME}
    ${GLOBAL_C}
    ${GLOBAL_H}
    ${INTERFACE_C}
    ${INTERFACE_H}
    ${MODULE_WIDGET_C}
    ${MODULE_WIDGET_H}
    ${MODULE_WIDGET_UI}
    ${SEARCH_LINEEDIT_C}
    ${SEARCH_LINEEDIT_H}
    ${RESOURCES}
    ${QM_FILES}
    ${MAIN_C}
    ${MAIN_H}
    )

target_link_libraries(${PROJECT_NAME}  Qt5::Core Qt5::Gui Qt5::Svg Qt5::X11Extras Qt5::Widgets dl kiranwidgets-qt5)

target_include_directories(${PROJECT_NAME} PRIVATE
    ../interface/
    ../global/
    ../widgets/
    src/widgets/kiran-application/
    src/kiran-style/)

#编译参数传入
include(GNUInstallDirs)
set(KCP_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
set(KCP_INSTALL_INCLUDE ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/)
set(KCP_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(KCP_INSTALL_DATADIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR})
set(KCP_PLUGIN_DESKTOP_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/plugins)
ADD_DEFINITIONS(-DPLUGIN_DESKTOP_DIR=\"${KCP_PLUGIN_DESKTOP_DIR}\")

#翻译
SET(TRANSLATE_PATH ${KCP_INSTALL_DATADIR}/${PROJECT_NAME}/translations/)
install(FILES ${QM_FILES} DESTINATION ${TRANSLATE_PATH})
ADD_DEFINITIONS(-DTRANSLATE_PREFIX=\"${TRANSLATE_PATH}${PROJECT_NAME}\")
#desktop and desktop image
file(GLOB_RECURSE desktop_files "app-desktop/files/*")
file(GLOB_RECURSE desktop_images "app-desktop/images/*")
set(KCP_DESKTOP_DIR ${KCP_INSTALL_DATADIR}/applications)
install(FILES ${desktop_files} DESTINATION ${KCP_DESKTOP_DIR})
set(KCP_DESKTOP_IMAGES_DIR ${KCP_INSTALL_DATADIR}/icons/Kiran/places/16x16/)
install(FILES ${desktop_images} DESTINATION ${KCP_DESKTOP_IMAGES_DIR})
ADD_DEFINITIONS(-DKCP_DESKTOP_IMAGES_PATH=\"${KCP_DESKTOP_IMAGES_DIR}${PROJECT_NAME}.svg\")
#安装
install(TARGETS ${PROJECT_NAME} DESTINATION ${KCP_INSTALL_BINDIR}/)

#开发头文件
install(FILES "../interface/module-interface.h" DESTINATION ${KCP_INSTALL_INCLUDE}/)

#pkgconfig
configure_file( kiran-control-center.pc.in kiran-control-center.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-control-center.pc
    DESTINATION ${KCP_INSTALL_LIBDIR}/pkgconfig/)
