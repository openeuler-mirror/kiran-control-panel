
file(GLOB src-code
    "*"
    )

set(QRC_SOURCE_FILES ../resources/image_control_center.qrc) #资源文件名不能与插件资源名相同，否则会导致插件中部分图片无法正常加载显示。
qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})

#翻译文件
find_package(Qt5 COMPONENTS Widgets LinguistTools)
file(GLOB TS_FILES "translations/*.ts")
qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

add_executable(${PROJECT_NAME}
    ${global}
    ${interface}
    ${kiran-module-widget}
    ${kiran-search-lineedit}
    ${src-code}
    ${QRC_FILES}
    ${QM_FILES}
    )

target_include_directories(${PROJECT_NAME} PRIVATE
    ../../interface/
    ../../global/
    ../../widgets/
    ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
    )

target_link_libraries(${PROJECT_NAME}  Qt5::Core Qt5::Gui Qt5::Svg Qt5::X11Extras Qt5::Widgets dl ${KIRAN_WIDGETS_QT5_LIBRARIES})

#安装
install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_BINDIR}/)

add_subdirectory(translations)
