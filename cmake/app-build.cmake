file(GLOB_RECURSE APP_BUILD_SOURCES "src/app_build/*.cpp")
file(GLOB_RECURSE APP_BUILD_HEADERS "src/app_build/*.h")
add_executable(${PROJECT_NAME}
        ${APP_BUILD_SOURCES}
        ${APP_BUILD_HEADERS}
        ${SRC}
        ${QM_FILES})
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
install(FILES "./config/${PROJECT_NAME}.desktop" DESTINATION ${SHARE_INSTALL_PREFIX}/applications/ )