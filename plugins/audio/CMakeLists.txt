set(TARGET_NAME kiran-cpanel-audio)

file(GLOB_RECURSE AUDIO_TRANSLATION_SRC ./*.cpp ./*.h ./*.ui)

file(GLOB_RECURSE COMMON_SRC common/*.cpp common/*.h)
file(GLOB_RECURSE DBUS_SRC dbus/*.cpp dbus/*.h)
file(GLOB_RECURSE PLUGIN_SRC plugin/*.cpp plugin/*.h plugin/*.ui)

add_library(${TARGET_NAME} SHARED
        ${COMMON_SRC}
        ${DBUS_SRC}
        ${PLUGIN_SRC})

target_include_directories(${TARGET_NAME} PRIVATE
        plugin
        dbus
        ${PROJECT_SOURCE_DIR}/common
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KLOG_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        Qt5::Widgets Qt5::DBus Qt5::Svg Qt5::Multimedia
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KIRAN_CC_DAEMON_LIBRARIES}
        ${KLOG_LIBRARIES})

install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_DIR}/)