####
#kiran-control-center下的目录结构：
#                    category                        #分类
#                            desktop                 #
#                            icons                   #
#
#                    plugins                         #插件
#                            desktop                 #
#                            icons                   #
#                            subitem-icons           #功能项图标  //这一项，插件可以自己定义，不一定放在这里。
#                            subitem-translations    #功能项翻译  //这一项，插件可以自己定义，不一定放在这里。
#
####

cmake_minimum_required(VERSION 3.0)
if(${SINGAL_PLUGIN})
    project(kiran-control-center-single-plugin VERSION 1.0)
    set(KIRAN_CONTROL_CENTER_VERSION     1.0.0)

else()
    project(kiran-control-center VERSION 1.0)
    set(KIRAN_CONTROL_CENTER_VERSION     1.0.0)
endif()

find_package(PkgConfig REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON) #会影响到插件中的资源寻址。
#
pkg_search_module(KIRANWIDGETS_QT5 REQUIRED kiranwidgets-qt5)
find_package(Qt5 COMPONENTS Widgets X11Extras Svg)

#编译参数传入
include(GNUInstallDirs)
set(INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
set(INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(INSTALL_DATADIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR})

#######################  由于cmakelist向下继承变量，所以这里提前申明宏定义。#######################
####start 需要与插件开发人员约定的路径。
set(KCP_INSTALL_INCLUDE ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/) #接口头文件安装路径，与pc文件中对应。
set(INSTALL_DATA_PROJECT_DIR ${INSTALL_DATADIR}/${PROJECT_NAME})

#category desktop
set(INSTALL_DATA_CATEGORY_DIR ${INSTALL_DATA_PROJECT_DIR}/category)
set(CATEGORY_DESKTOP_DIR ${INSTALL_DATA_CATEGORY_DIR}/desktop)
#ADD_DEFINITIONS(-DCATEGORY_DESKTOP_DIR=\"${CATEGORY_DESKTOP_DIR}\")
set(CATEGORY_DESKTOP_ICON_DIR ${INSTALL_DATA_CATEGORY_DIR}/icons)
#ADD_DEFINITIONS(-DCATEGORY_DESKTOP_ICON_DIR=\"${CATEGORY_DESKTOP_ICON_DIR}\")

#plugins desktop path
set(INSTALL_DATA_PLUGIN_DIR ${INSTALL_DATA_PROJECT_DIR}/plugins)
#ADD_DEFINITIONS(-DPLUGIN_DESKTOP_DIR=\"${INSTALL_DATA_PLUGIN_DIR}/desktop\") #插件的desktop安装路径。这个需要打印输出。
#ADD_DEFINITIONS(-DPLUGIN_DESKTOP_ICON_DIR=\"${INSTALL_DATA_PLUGIN_DIR}/icons\") #插件的图标安装路径。这个也需要打印输出。
####end

#app
###desktop
set(SYSTEM_IMAGES_DIR ${INSTALL_DATADIR}/${PROJECT_NAME}/icons/)  #app图标安装路径。
#ADD_DEFINITIONS(-DAPP_IMAGE_PATH=\"${SYSTEM_IMAGES_DIR}${PROJECT_NAME}.svg\") #app加载图标路径。
###翻译
SET(INSTALL_DATA_TRANSLATE_DIR ${INSTALL_DATA_PROJECT_DIR}/translations/)  #app翻译文件安装路径。
#ADD_DEFINITIONS(-DTRANSLATE_PREFIX=\"${INSTALL_DATA_TRANSLATE_DIR}${PROJECT_NAME}\")  #app加载翻译文件路径。

configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h)
#config.h
include_directories(${CMAKE_BINARY_DIR})
##########################      end         #################################

#定义公用代码文件集
file(GLOB global "global/*"  )
file(GLOB interface "interface/*" )
file(GLOB kiran-module-widget "widgets/kiran-module-widget/*" )
file(GLOB kiran-search-lineedit "widgets/kiran-search-lineedit/*" )

add_subdirectory(interface)
add_subdirectory(data)

if(${SINGAL_PLUGIN})
    add_subdirectory(plugin-single)

else()
    add_subdirectory(plugin-group)

endif()







