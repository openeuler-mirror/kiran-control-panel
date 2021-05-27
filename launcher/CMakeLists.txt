cmake_minimum_required(VERSION 3.5)
project(kiran-cpanel-launcher)

find_package(Qt5 COMPONENTS Widgets LinguistTools)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(GLIB_2 REQUIRED glib-2.0)

file(GLOB_RECURSE LAUNCHER_SRC
        "./src/*.cpp"
        "./src/*.h")

set(LAUNCHER_TS "translations/kiran-cpanel-launcher.zh_CN.ts")
qt5_create_translation(LAUNCHER_QM ${CMAKE_SOURCE_DIR} ${LAUNCHER_TS})

add_executable(${PROJECT_NAME}
        ${common}
        ${LAUNCHER_SRC}
        ${LAUNCHER_QM})

target_include_directories(${PROJECT_NAME} PRIVATE
        ../../include/
        ../../global/
        ../../widgets/
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${GLIB_2_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Gui
        Qt5::Svg
        Qt5::X11Extras
        Qt5::Widgets
        dl
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES}
        ${GLIB_2_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
install(FILES ${LAUNCHER_QM} DESTINATION ${TRANSLATION_INSTALL_DIR})
