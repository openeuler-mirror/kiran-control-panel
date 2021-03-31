#读取公用 pc 文件
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
include_directories(${KIRAN_CC_DAEMON_INCLUDE_DIRS})
#
find_package(Qt5 COMPONENTS Widgets DBus X11Extras Svg) #
#定义公用代码文件集
file(GLOB SRC
    "./*.cpp"
    "./*.h"
    "./*.ui"
    )
#翻译文件
include_directories("${CMAKE_BINARY_DIR}/src")
file(GLOB TS_FILES "translations/*.ts")
#config.h
include_directories(${CMAKE_BINARY_DIR})

set(QRC_SOURCE_FILES ../resources/res.qrc)
qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})

find_package(Qt5 COMPONENTS Widgets LinguistTools)
SET_DIRECTORY_PROPERTIES(PROPERTIES CLEAN_NO_CUSTOM TRUE)
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

if(DEFINED PLUGIN_DIR)
    message("********build plugin.*********")
    pkg_search_module(KIRAN_CONTROL_CENTER REQUIRED kiran-control-center)
    include_directories(${KIRAN_CONTROL_CENTER_INCLUDE_DIRS})

    file(GLOB PLUGIN
        "plugin/*.cpp"
        "plugin/*.h"
        )

    add_library(${PROJECT_NAME} SHARED
        ${SRC}
        ${PLUGIN}
        ${QRC_FILES}
        ${QM_FILES}
        )
    #安装
    install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_LIBDIR}/)

else()
    message("********** build app. *********")

    file(GLOB APP
        "app/*.cpp"
        "app/*.h"
        )

    add_executable(${PROJECT_NAME}
        ${SRC}
        ${APP}
        ${QRC_FILES}
        ${QM_FILES}
        )

    #安装
    install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_BINDIR}/)

endif()

target_link_libraries(${PROJECT_NAME}
    Qt5::Core Qt5::Gui Qt5::Svg Qt5::Widgets Qt5::X11Extras Qt5::DBus pthread ${ZLOG_LIBRARIES} ${KIRAN_WIDGETS_QT5_LIBRARIES} X11)

include(app_plugin_translate)
