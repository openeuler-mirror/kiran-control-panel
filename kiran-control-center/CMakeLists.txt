cmake_minimum_required(VERSION 3.5)

project(kiran-control-panel)
set(KIRAN_CONTROL_PANEL_VERSION     1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(PluginDesktopInstallDir ${SHARE_INSTALL_DIR}/${PROJECT_NAME}/plugins)
ADD_DEFINITIONS(-DPLUGIN_DESKTOP_DIR=\"${PluginDesktopInstallDir}\")
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
file(GLOB_RECURSE RESOURCES "*.qrc")


#qt5_add_resources(qrc_FILES drawable.qrc)

find_package(Qt5 COMPONENTS Core Gui Svg Widgets X11Extras REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(KIRANWIDGETS_QT5 REQUIRED kiranwidgets-qt5)


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
    ${MAIN_C}
    ${MAIN_H}
    ${RESOURCES}
    )

target_link_libraries(${PROJECT_NAME}  Qt5::Core Qt5::Gui Qt5::Svg Qt5::X11Extras Qt5::Widgets dl kiranwidgets-qt5)

target_include_directories(${PROJECT_NAME} PRIVATE
    ../interface/
    ../global/
    ../widgets/
    src/widgets/kiran-application/
    src/kiran-style/)


#安装SO
install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR}/)

#开发头文件
install(FILES "../interface/module-interface.h" DESTINATION ${INCLUDE_INSTALL_DIR}/${PROJECT_NAME}/)

#pkgconfig
configure_file( kiran-control-panel.pc.in kiran-control-panel.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-control-panel.pc
    DESTINATION ${INSTALL_DIR}/pkgconfig/)
