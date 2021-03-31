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
    message("*********** build kiran-control-center-single-plugin ***********")
    project(kiran-control-center-single-plugin VERSION 1.0)
    set(KIRAN_CONTROL_CENTER_VERSION     1.0.0)

else()
    message("*********** build kiran-control-center ***********")
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
pkg_search_module(ZLOG REQUIRED zlog)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
find_package(Qt5 COMPONENTS Widgets X11Extras Svg)

#编译参数传入
include(GNUInstallDirs)
set(INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
set(INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(INSTALL_DATADIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR})

#######################  由于cmakelist向下继承变量，所以这里提前申明宏定义。#######################
#-------------   需要与插件开发人员约定的路径。  -------------#
set(KCP_INSTALL_INCLUDE ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/) #接口头文件安装路径，与pc文件中对应。
if(${SINGAL_PLUGIN})
    set(INSTALL_DATA_PLUGIN_DIR ${INSTALL_DATADIR}/kiran-control-center) #单独运行时，也是去kiran-control-center目录下加载插件。
else()
    set(INSTALL_DATA_PLUGIN_DIR ${INSTALL_DATADIR}/${PROJECT_NAME})
endif()

#category desktop
set(INSTALL_DATA_CATEGORY_DIR ${INSTALL_DATA_PLUGIN_DIR}/category)
set(CATEGORY_DESKTOP_DIR ${INSTALL_DATA_CATEGORY_DIR}/desktop)
set(CATEGORY_DESKTOP_ICON_DIR ${INSTALL_DATA_CATEGORY_DIR}/icons)

#plugins desktop path
set(INSTALL_DATA_PLUGIN_DIR ${INSTALL_DATA_PLUGIN_DIR}/plugins)
#-------------------------------------------------#

#app
###desktop
set(SYSTEM_IMAGES_DIR ${INSTALL_DATADIR}/${PROJECT_NAME}/icons/)  #app图标安装路径。
###翻译
SET(INSTALL_DATA_TRANSLATE_DIR ${INSTALL_DATADIR}/${PROJECT_NAME}/translations/)  #app翻译文件安装路径。

#config.h
configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h)
include_directories(${CMAKE_BINARY_DIR})
##########################      end         #################################

#定义公用代码文件集
file(GLOB global "global/*"  )
file(GLOB interface "interface/*" )
file(GLOB kiran-module-widget "widgets/kiran-module-widget/*" )
file(GLOB kiran-search-lineedit "widgets/kiran-search-lineedit/*" )

if(${SINGAL_PLUGIN})
    add_subdirectory(plugin-single)

else()
    add_subdirectory(interface) #开发插件时，需要在正常状态下编译控制中心框架。
    add_subdirectory(data)
    add_subdirectory(plugin-group)

endif()







