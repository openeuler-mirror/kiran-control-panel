set(TARGET_NAME kiran-cpanel-font)

pkg_search_module(FONTCONFIG REQUIRED fontconfig)

# fontconfig 2.18 起 FcFreeTypeQueryAll 只在 fcfreetype.h 中声明（实际拆分发生在 2.18.1），
# 该头依赖 freetype 头文件，故 2.18 及以上要求提供 freetype2
if (NOT FONTCONFIG_VERSION VERSION_LESS 2.18.0)
    pkg_search_module(FREETYPE REQUIRED freetype2)
endif()

file(GLOB FONT_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.h)

add_library(${TARGET_NAME} SHARED
        ${FONT_SRC})

target_include_directories(${TARGET_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/common
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_BINARY_DIR}
        ${KLOG_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_INCLUDE_DIRS}
        ${KIRAN_INTEGRATION_THEME_INCLUDE_DIRS}
        ${FONTCONFIG_INCLUDE_DIRS}
        ${FREETYPE_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        plugin-devel
        Qt5::Widgets
        Qt5::Core
        ${KIRAN_WIDGETS_LIBRARIES}
        ${KLOG_LIBRARIES}
        ${KIRAN_INTEGRATION_THEME_LIBRARIES}
        ${FONTCONFIG_LIBRARIES})

install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_DIR}/)

add_subdirectory(helper)
