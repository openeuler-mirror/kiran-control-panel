

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
file(GLOB_RECURSE RESOURCES "*.qrc")
file(GLOB_RECURSE TS_FILES "*.ts")

find_package(Qt5 COMPONENTS Widgets DBus X11Extras Svg )


if(${BUILD_APP})
    add_executable(${PROJECT_NAME}
        ${PUBLIC_CPP}
        ${PUBLIC_H}
        ${RESOURCES}
        ${TS_FILES})
else()
    add_library(${PROJECT_NAME} SHARED
        ${PUBLIC_CPP}
        ${PUBLIC_H}
        ${RESOURCES}
        ${TS_FILES})
endif()


target_include_directories(${PROJECT_NAME} PRIVATE
    widgets/kiran-application/)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core Qt5::Gui Qt5::Svg Qt5::Widgets Qt5::X11Extras Qt5::DBus kiranwidgets-qt5 X11)

#安装
install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_INSTALL_DIR}/)

#翻译
install(FILES ${TS_FILES} DESTINATION ${SHARE_INSTALL_PREFIX}/${PROJECT_NAME}/translations/)


