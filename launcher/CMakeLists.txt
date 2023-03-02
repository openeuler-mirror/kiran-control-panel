set(TARGET_NAME kiran-cpanel-launcher)

file(GLOB_RECURSE LAUNCHER_SRC "./src/*.cpp" "./src/*.h")
set(CONTROL_PANEL_QRC ../resources/control-panel-resources.qrc)
qt5_create_translation(LAUNCHER_QM ${CMAKE_SOURCE_DIR} translations/kiran-cpanel-launcher.zh_CN.ts)

add_executable(${TARGET_NAME}
        ${LAUNCHER_SRC}
        ${LAUNCHER_QM}
        ${CONTROL_PANEL_QRC})

target_include_directories(${TARGET_NAME} PRIVATE
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KLOG_INCLUDE_DIRS}
        ${PROJECT_BINARY_DIR})

target_link_libraries(${TARGET_NAME}
        common-widgets
        plugin-framework
        Qt5::Core
        Qt5::Gui
        Qt5::Svg
        Qt5::X11Extras
        Qt5::Widgets
        dl
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KLOG_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
install(FILES ${LAUNCHER_QM} DESTINATION ${TRANSLATION_INSTALL_DIR})
