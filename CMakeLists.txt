

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

find_package(Qt5 COMPONENTS Widgets DBus X11Extras Svg LinguistTools)
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})


if(${BUILD_APP})
    add_executable(${PROJECT_NAME}
        ${PUBLIC_CPP}
        ${PUBLIC_H}
        ${RESOURCES}
        ${QM_FILES})

    #翻译
    install(FILES ${QM_FILES} DESTINATION ${SHARE_INSTALL_PREFIX}/${PROJECT_NAME}/translations/)
    ADD_DEFINITIONS(-DTRANSLATE_PREFIX=\"${SHARE_INSTALL_PREFIX}/${PROJECT_NAME}/translations/${PROJECT_NAME}\")
else()
    add_library(${PROJECT_NAME} SHARED
        ${PUBLIC_CPP}
        ${PUBLIC_H}
        ${RESOURCES}
        ${QM_FILES})

    #翻译
    install(FILES ${QM_FILES} DESTINATION ${SHARE_INSTALL_PREFIX}/kiran-control-panel/plugins/${PROJECT_NAME}/translate/)
    ADD_DEFINITIONS(-DTRANSLATE_PREFIX=\"${SHARE_INSTALL_PREFIX}/kiran-control-panel/plugins/${PROJECT_NAME}/translate/${PROJECT_NAME}\")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    widgets/kiran-application/)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core Qt5::Gui Qt5::Svg Qt5::Widgets Qt5::X11Extras Qt5::DBus kiranwidgets-qt5 X11)

#安装
install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR}/)




