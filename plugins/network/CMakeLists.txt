cmake_minimum_required(VERSION 3.17)
project(kiran-cpanel-network)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Gui Widgets DBus Network Svg)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets Qt5::DBus Qt5::Network Qt5::Svg)

file(GLOB_RECURSE SRC "./src/*.cpp" "./src/*.h" "./src/*.ui")
file(GLOB_RECURSE QRC "./resources/*.qrc")

add_executable(${PROJECT_NAME}
        ${SRC}
        ${QRC}
        ./main.cpp
        )

MESSAGE(${PROJECT_NAME})

find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
find_package(KF5NetworkManagerQt REQUIRED)


find_package(PkgConfig REQUIRED)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(LIBNOTIFY REQUIRED libnotify)

target_include_directories(${PROJECT_NAME} PRIVATE
        ./src
        ./src/plugin/manager
        ./src/plugin/settings
        ./src/plugin/settings/vpn
        ./src/plugin/setting-widget
        ./src/plugin/setting-widget/vpn
        #暂时添加tray方便调试，之后修改
        ./src/tray
        ./src/tray/dbus
        ./resources
        ./resources/kcp-network-images
        ./resources/kcp-network-themes
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${LIBNOTIFY_INCLUDE_DIRS}
        )

target_link_libraries(${PROJECT_NAME}
        ${REQUIRED_LIBS_QUALIFIED}
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES}
        ${LIBNOTIFY_LIBRARIES}
        KF5::NetworkManagerQt
        )

#set(TRAY kiran-network-status-icon)
#add_executable(${TRAY}
#        main.cpp
#        )
#
#target_include_directories(${TRAY} PRIVATE
#        )
#
#target_link_libraries(${TRAY}
#        )
