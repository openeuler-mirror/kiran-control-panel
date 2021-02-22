set(KCP_PLUGIN_DESKTOP_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/plugins)
ADD_DEFINITIONS(-DPLUGIN_DESKTOP_DIR=\"${KCP_PLUGIN_DESKTOP_DIR}\")

file(GLOB src-code
    "*.cpp"
    "*.h"
    )

add_executable(${PROJECT_NAME}
    ${global}
    ${interface}
    ${kiran-module-widget}
    ${kiran-search-lineedit}
    ${src-code}
    )

target_include_directories(${PROJECT_NAME} PRIVATE
    ../../interface/
    ../../global/
    ../../widgets/
    src/widgets/kiran-application/
    src/kiran-style/
    )

target_link_libraries(${PROJECT_NAME}  Qt5::Core Qt5::Gui Qt5::Svg Qt5::X11Extras Qt5::Widgets dl kiranwidgets-qt5)

#安装
install(TARGETS ${PROJECT_NAME} DESTINATION ${KCP_INSTALL_BINDIR}/)
