set(TARGET_NAME kiran-cpanel-printer)

file(GLOB_RECURSE PRINTER_SRC ./*.cpp ./*.h ./*.ui)

find_library(CUPS_LIBRARY cups)
if (NOT CUPS_LIBRARY)
    message(FATAL_ERROR "cups library not found")
endif ()

add_library(${TARGET_NAME} SHARED ${PRINTER_SRC})

target_include_directories(
  ${TARGET_NAME}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/pages
          ${CMAKE_CURRENT_SOURCE_DIR}/wrapper
          ${PROJECT_BINARY_DIR}
          ${CMAKE_CURRENT_BINARY_DIR}
          ${KCP_PLUGIN_INCLUDE_DIR}
          ${KIRAN_WIDGETS_INCLUDE_DIRS}
          ${KLOG_INCLUDE_DIRS}
          ${KIRAN_STYLE_INCLUDE_DIRS})

target_link_libraries(
  ${TARGET_NAME}
  common-widgets
  Qt5::Widgets
  Qt5::DBus
  Qt5::Svg
  Qt5::Core
  ${CUPS_LIBRARY}
  ${KIRAN_WIDGETS_LIBRARIES}
  ${KLOG_LIBRARIES}
  ${KIRAN_STYLE_LIBRARIES})

set(KCP_OPEN_NEW_PRINTER_HELPER "${CMAKE_INSTALL_FULL_LIBEXECDIR}/kiran-cpanel-open-new-printer")
target_compile_definitions(${TARGET_NAME} PRIVATE KCP_OPEN_NEW_PRINTER_HELPER="${KCP_OPEN_NEW_PRINTER_HELPER}")

install(TARGETS ${TARGET_NAME} DESTINATION ${PLUGIN_LIBS_DIR}/)
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/helper/open-new-printer-dialog.py
        DESTINATION ${CMAKE_INSTALL_FULL_LIBEXECDIR}
        RENAME kiran-cpanel-open-new-printer)
