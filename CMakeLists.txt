

cmake_minimum_required(VERSION 3.5)

project(kiran-display-tools)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE PUBLIC_CPP "*.cpp")
file(GLOB_RECURSE PUBLIC_H "*.h")

set(RCC_FILES *.qrc)
set(TS_FILES kiran-display-tools.zh_CN.ts)

include_directories("${CMAKE_BINARY_DIR}")

find_package(Qt5 COMPONENTS Widgets DBus X11Extras)

#qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

#if(DFL_BUILD_SHARED)
#add_library(flFTP SHARED flFTP.cpp flFTP.h)
#else()
#add_library(flFTP STATIC flFTP.cpp flFTP.h)
#endif()


#    add_executable(${PROJECT_NAME}
#        ${PUBLIC_CPP}
#        ${PUBLIC_H}
#        ${RESOURCES}
#        ${QM_FILES})

#    add_library(${PROJECT_NAME} SHARED
#        ${PUBLIC_CPP}
#        ${PUBLIC_H}
#        ${RESOURCES}
#        ${QM_FILES})



target_include_directories(${PROJECT_NAME} PRIVATE
    widgets/
    public/
    widgets/kiran-application/
    ${GSETTINGS_QT_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    Qt5::X11Extras
    Qt5::DBus
    kiranwidgets-qt5
    X11
    xcb
    xcb-util
    ${GSETTINGS_QT_LIBRARIES})

##安装SO
#install(TARGETS ${PROJECT_NAME} DESTINATION ${LIB_INSTALL_DIR}/)

##开发头文件
#install(FILES ${DEVEL_HEADER} DESTINATION ${INCLUDE_INSTALL_DIR}/${PROJECT_NAME}/)

##翻译
#install(FILES ${QM_FILES} DESTINATION ${SHARE_INSTALL_PREFIX}/${PROJECT_NAME}/translations/)


