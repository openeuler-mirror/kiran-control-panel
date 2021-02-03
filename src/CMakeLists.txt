#读取公用 pc 文件
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
include_directories(${KIRAN_CC_DAEMON_INCLUDE_DIRS})
#
find_package(Qt5 COMPONENTS Widgets DBus X11Extras Svg) #
#定义公用代码文件集
file(GLOB SRC
    "./kiran-*.cpp"
    "./kiran-*.h"
    "./kiran-*.ui"
    "../images/*.qrc"
    )
#翻译文件
file(GLOB TS_FILES "translations/*.ts")
find_package(Qt5 COMPONENTS Widgets LinguistTools)
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
        ${QM_FILES}
        )
    #安装
    install(TARGETS ${PROJECT_NAME} DESTINATION ${KDT_INSTALL_LIBDIR}/)

    target_link_libraries(${PROJECT_NAME}
        Qt5::Core Qt5::Gui Qt5::Svg Qt5::Widgets Qt5::X11Extras Qt5::DBus X11)

else()
    message("********** build app. *********")
    pkg_search_module(KIRANWIDGETS_QT5 REQUIRED kiranwidgets-qt5)
    include_directories(${KIRANWIDGETS_QT5_INCLUDE_DIRS})
    link_directories(${KIRANWIDGETS_QT5_LIBRARY_DIRS})

    file(GLOB APP
        "app/*.cpp"
        "app/*.h"
        )

    add_executable(${PROJECT_NAME}
        ${SRC}
        ${APP}
        ${QM_FILES}
        )

    #安装
    install(TARGETS ${PROJECT_NAME} DESTINATION ${KDT_INSTALL_BINDIR}/)

    target_link_libraries(${PROJECT_NAME}
        Qt5::Core Qt5::Gui Qt5::Svg Qt5::Widgets Qt5::X11Extras Qt5::DBus kiranwidgets-qt5 X11)

endif()

add_subdirectory(translations)
