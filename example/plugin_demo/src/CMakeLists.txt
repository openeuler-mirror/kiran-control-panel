#glib
pkg_search_module(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})
#gsettings-qt
pkg_search_module(GSETTINGS_QT REQUIRED gsettings-qt)
include_directories(${GSETTINGS_QT_INCLUDE_DIRS})
link_directories(${GSETTINGS_QT_LIBRARY_DIRS})
#
find_package(Qt5 COMPONENTS Widgets X11Extras Svg Xml)
#定义公用代码文件集
file(GLOB SRC
    "./*.cpp"
    "./*.h"
    "./*.ui"
    "../images/*.qrc"
    )
#翻译文件
include_directories("${CMAKE_BINARY_DIR}/src")
file(GLOB TS_FILES "translations/*.ts")
#config.h
include_directories(${CMAKE_BINARY_DIR})

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
        ${QM_FILES}
        )
    #安装
    install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_LIBDIR}/)

    target_link_libraries(${PROJECT_NAME}
        Qt5::Core Qt5::Gui Qt5::Svg Qt5::Widgets Qt5::X11Extras Qt5::Xml X11 glib-2.0 gio-2.0 gobject-2.0 gsettings-qt)

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
    install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_BINDIR}/)

    target_link_libraries(${PROJECT_NAME}
        Qt5::Core Qt5::Gui Qt5::Svg Qt5::Widgets Qt5::X11Extras Qt5::Xml kiranwidgets-qt5 X11 glib-2.0 gio-2.0 gobject-2.0 gsettings-qt)

endif()

include(app_plugin_translate)
