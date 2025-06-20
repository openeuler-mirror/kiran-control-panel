set(TARGET_NAME kiran-cpanel-appearance)

if (ENABLE_APPEARANCE_SCREENSAVER)
    add_definitions(-DAPPEARANCE_SCREENSAVER)
endif ()

file(GLOB_RECURSE APPEARANCE_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

add_library(${TARGET_NAME} SHARED ${APPEARANCE_SRC} ${APPEARANCE_PROXY})

target_include_directories(${TARGET_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/common
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common-widgets
        dbus-wrapper
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::Core
        Xcursor
        X11
        ${KIRANWIDGETS_LIBRARIES}
        ${KLOG_LIBRARIES}
        ${KIRAN_STYLE_LIBRARIES})

install(TARGETS ${TARGET_NAME}
        DESTINATION ${PLUGIN_LIBS_DIR}/)