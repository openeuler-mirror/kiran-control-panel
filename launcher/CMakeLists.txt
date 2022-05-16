cmake_minimum_required(VERSION 3.2)

set(TARGET_NAME kiran-cpanel-launcher)

find_package(Qt5 COMPONENTS Widgets LinguistTools)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)

file(GLOB_RECURSE LAUNCHER_SRC
        "./src/*.cpp"
        "./src/*.h")

set(CONTROL_PANEL_QRC ../resources/control-panel-resources.qrc)
qt5_create_translation(LAUNCHER_QM ${CMAKE_SOURCE_DIR} translations/kiran-cpanel-launcher.zh_CN.ts)

add_executable(${TARGET_NAME}
        ${common}
        ${LAUNCHER_SRC}
        ${LAUNCHER_QM}
        ${CONTROL_PANEL_QRC})

target_include_directories(${TARGET_NAME} PRIVATE
        ../../global/
        ../../widgets/
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${PROJECT_BINARY_DIR})

target_link_libraries(${TARGET_NAME}
        common-widgets
        plugin-info
        Qt5::Core
        Qt5::Gui
        Qt5::Svg
        Qt5::X11Extras
        Qt5::Widgets
        dl
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
install(FILES ${LAUNCHER_QM} DESTINATION ${TRANSLATION_INSTALL_DIR})
