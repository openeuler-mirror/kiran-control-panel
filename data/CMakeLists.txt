#pkgconfig
configure_file( kiran-control-center.pc.in kiran-control-center.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kiran-control-center.pc
    DESTINATION ${INSTALL_LIBDIR}/pkgconfig/)
