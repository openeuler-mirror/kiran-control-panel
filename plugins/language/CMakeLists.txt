set(TARGET_NAME kiran-cpanel-language)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg DBus LinguistTools Network)
pkg_search_module(KIRAN_WIDGETS_QT5 REQUIRED kiranwidgets-qt5)
pkg_search_module(KLOG_QT5 REQUIRED klog-qt5)
pkg_search_module(KIRAN_CC_DAEMON REQUIRED kiran-cc-daemon)
pkg_search_module(KIRAN_CONTROL_PANEL REQUIRED kiran-control-panel)
pkg_search_module(KIRAN_STYLE_HELPER REQUIRED kiran-style-helper)
pkg_search_module(QGSETTINGS REQUIRED gsettings-qt)
pkg_search_module(LIBNOTIFY REQUIRED libnotify)
pkg_search_module(PACKAGEKIT_QT5 REQUIRED packagekitqt5)

set(LANGUAGE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib/common-widgets/kiran-setting-container ${PROJECT_SOURCE_DIR}/src/search-edit)

file(GLOB_RECURSE LANGUAGE_PLUGIN_SRC
                  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

add_library(${TARGET_NAME} SHARED
        ${LANGUAGE_PLUGIN_SRC})

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KCP_PLUGIN_INCLUDE_DIR}
        ${LANGUAGE_INCLUDE_DIRS}
        ${KIRAN_WIDGETS_QT5_INCLUDE_DIRS}
        ${KLOG_QT5_INCLUDE_DIRS}
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KIRAN_STYLE_HELPER_INCLUDE_DIRS}
        ${QGSETTINGS_INCLUDE_DIRS}
        ${LIBNOTIFY_INCLUDE_DIRS}
        ${PACKAGEKIT_QT5_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME}
        common-widgets
        Qt5::Widgets
        Qt5::DBus
        Qt5::Svg
        Qt5::Core
        ${KIRAN_WIDGETS_QT5_LIBRARIES}
        ${KLOG_QT5_LIBRARIES}
        ${KIRAN_STYLE_HELPER_LIBRARIES}
        ${QGSETTINGS_LIBRARIES}
        ${LIBNOTIFY_LIBRARIES}
        ${PACKAGEKIT_QT5_LIBRARIES})

install(TARGETS ${TARGET_NAME}
        DESTINATION ${PLUGIN_LIBS_DIR}/)

install(FILES ${LANGUAGE_QM} DESTINATION ${TRANSLATION_DIR}/)

set(SUPPORT_LANGUAGE_JSON support_language.json)
set(KIRAN_SUPPORT_LANGUAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(KIRAN_SYSTEM_SUPPORT_LANGUAGE_DIR ${PLUGIN_DIR}/data)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/${SUPPORT_LANGUAGE_JSON}
        DESTINATION ${PLUGIN_DIR}/data/)

set(SCHEMA_IN_FILE com.kylinsec.kiran.language.gschema.xml.in)
set(SCHEMA_FILE com.kylinsec.kiran.language.gschema.xml)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/${SCHEMA_IN_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${SCHEMA_FILE})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${SCHEMA_FILE}
        DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/glib-2.0/schemas/)
install(
        CODE "execute_process(COMMAND glib-compile-schemas \"${CMAKE_INSTALL_FULL_DATADIR}/glib-2.0/schemas/\")"
  )


