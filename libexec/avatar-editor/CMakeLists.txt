set(TARGET_NAME kiran-avatar-editor)

file(GLOB_RECURSE AVATAR_EDITOR_SRC "*.cpp" "*.h" "*.ui")
set(AVATAR_EDITOR_RESOURCES resources/avatar-editor-resources.qrc)
file(GLOB TS_FILES "translations/*.ts")
qt5_create_translation( AVATAR_EDITOR_QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES} )

add_executable(kiran-avatar-editor ${AVATAR_EDITOR_SRC} ${AVATAR_EDITOR_RESOURCES} ${AVATAR_EDITOR_QM_FILES})

target_include_directories(kiran-avatar-editor PRIVATE
        ${PROJECT_SOURCE_DIR}/common
        ${CMAKE_BINARY_DIR}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_INTEGRATION_THEME_INCLUDE_DIRS})

target_link_libraries(kiran-avatar-editor
        Qt5::Widgets
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KLOG_LIBRARIES}
        ${KIRAN_INTEGRATION_THEME_LIBRARIES})

install(TARGETS kiran-avatar-editor DESTINATION ${CMAKE_INSTALL_FULL_LIBEXECDIR})
install(FILES ${AVATAR_EDITOR_QM_FILES} DESTINATION ${TRANSLATION_DIR} )
