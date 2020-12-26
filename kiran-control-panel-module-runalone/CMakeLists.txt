cmake_minimum_required(VERSION 3.2)

project(kiran-control-panel-module-runalone)

file(GLOB_RECURSE INTERFACE_H_FILES ../interface/*.h)
file(GLOB_RECURSE INTERFACE_CPP_FILES ../interface/*.cpp)
file(GLOB_RECURSE GLOBAL_H_FILES ../global/*.h)
file(GLOB_RECURSE GLOBAL_CPP_FILES ../global/*.cpp)
file(GLOB_RECURSE WIDGET_H_FILES ../widgets/*.h)
file(GLOB_RECURSE WIDGET_CPP_FILES ../widgets/*.cpp)

file(GLOB_RECURSE GREETER_SETTINGS_CPP_FILES ./*.cpp)
file(GLOB_RECURSE GREETER_SETTINGS_H_FILES ./*.h)

find_package(Qt5 COMPONENTS Widgets Svg X11Extras LinguistTools)

include_directories(/usr/include)

add_executable(${PROJECT_NAME}
    ${INTERFACE_H_FILES}
    ${INTERFACE_CPP_FILES}
    ${GLOBAL_H_FILES}
    ${GLOBAL_CPP_FILES}
    ${WIDGET_H_FILES}
    ${WIDGET_CPP_FILES}
    ${GREETER_SETTINGS_H_FILES}
    ${GREETER_SETTINGS_CPP_FILES}
    )

target_include_directories(${PROJECT_NAME} PRIVATE
        ./
        ../interface/
        ../global/
        ../widgets/
        ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
        ${Qt5Widgets_PRIVATE_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Widgets
    Qt5::Svg
    Qt5::X11Extras
    X11
    xcb
    xcb-util
    kiranwidgets-qt5
    ${GSETTINGS_QT_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_INSTALL_DIR}/)
