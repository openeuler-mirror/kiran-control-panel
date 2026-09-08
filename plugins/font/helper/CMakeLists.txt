set(FONT_HELPER_TARGET kiran-cpanel-font-helper)
set(FONT_HELPER_PATH ${CMAKE_INSTALL_FULL_LIBEXECDIR}/${FONT_HELPER_TARGET})

add_executable(${FONT_HELPER_TARGET}
        main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../font-utils.cpp)

target_include_directories(${FONT_HELPER_TARGET} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_BINARY_DIR}
        ${FONTCONFIG_INCLUDE_DIRS})

target_link_libraries(${FONT_HELPER_TARGET}
        Qt5::Core
        ${FONTCONFIG_LIBRARIES})

install(TARGETS ${FONT_HELPER_TARGET} DESTINATION ${CMAKE_INSTALL_FULL_LIBEXECDIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/com.kylinsec.kiran.control-panel.font.policy.in
        ${CMAKE_CURRENT_BINARY_DIR}/com.kylinsec.kiran.control-panel.font.policy
        @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/com.kylinsec.kiran.control-panel.font.policy
        DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/polkit-1/actions/)
