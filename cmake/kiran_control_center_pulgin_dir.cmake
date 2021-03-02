if(BUILD_PLUGIN)
    macro(GET_PLUGIN_DIR _PLUGIN_DIR)
        find_program(KIRAN_CONTROL_CENTER_FOUND kiran-control-center)

        if (!KIRAN_CONTROL_CENTER_FOUND)
            message(FATAL_ERROR "not found kiran-control-center.")
        endif()

        execute_process(
            COMMAND kiran-control-center --print-plugins-dir
            OUTPUT_VARIABLE ${_PLUGIN_DIR}
            OUTPUT_STRIP_TRAILING_WHITESPACE
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            )
    endmacro()
    set(PLUGIN_DIR "unknown")
    GET_PLUGIN_DIR(PLUGIN_DIR)
    message(STATUS "PLUGIN_DIR: ${PLUGIN_DIR}")
endif()
