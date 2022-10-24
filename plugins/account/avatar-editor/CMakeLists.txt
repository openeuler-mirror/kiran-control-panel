cmake_minimum_required(VERSION 3.2)

set(TARGET_NAME kiran-avatar-editor)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets LinguistTools)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_STYLE_HELPER REQUIRED kiran-style-helper)

file(GLOB_RECURSE SOURCES   src/*.cpp)
file(GLOB_RECURSE HEADERS   src/*.h)
file(GLOB_RECURSE FORMS     src/*.ui)
file(GLOB_RECURSE RESOURCES resources/*.qrc)

qt5_create_translation( AVATAR_EDITOR_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} translations/kiran-avatar-editor.zh_CN.ts )

add_executable(kiran-avatar-editor ${SOURCES} ${HEADERS} ${RESOURCES} ${AVATAR_EDITOR_QM_FILES})

target_include_directories(kiran-avatar-editor PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/..
        ../include
        include
        tools
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS})

target_link_libraries(kiran-avatar-editor
        Qt5::Widgets
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES})

install(TARGETS kiran-avatar-editor DESTINATION ${KCP_ACCOUNT_AVATAR_EDITOR})
install(FILES ${AVATAR_EDITOR_QM_FILES} DESTINATION ${TRANSLATION_INSTALL_DIR} )
