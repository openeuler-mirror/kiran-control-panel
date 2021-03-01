function(GET_CONTROL_CENTER_DIR  DESKTOP_DIR_OUTPUT ICON_DIR_OUTPUT )

    #获取kiran-control-center全路径
    set(KIRAN_CONTROL_CENTER_NAME "kiran-control-center")
    find_program(KIRAN_CONTROL_CENTER ${KIRAN_CONTROL_CENTER_NAME})
    if( NOT KIRAN_CONTROL_CENTER)
        message(FATAL_ERROR "not find ${KIRAN_CONTROL_CENTER_NAME}")
    endif()

    #执行命令
    execute_process(COMMAND "${KIRAN_CONTROL_CENTER}" "--print-plugins-dir"
                    TIMEOUT 1
                    OUTPUT_VARIABLE PLUGINS_DIR_OUTPUT)
    string(REPLACE "\n" ";" OUTPUT_LIST ${PLUGINS_DIR_OUTPUT})

    #遍历输出寻找目录
    foreach(LINE_OUTPUT ${OUTPUT_LIST})
        set(DESKTOP_DIR_KEYWORD "Plugin desktop dir: ")
        string(FIND ${LINE_OUTPUT} ${DESKTOP_DIR_KEYWORD} CAN_FIND)
        if( NOT (${CAN_FIND} EQUAL -1) )
            string(REPLACE ${DESKTOP_DIR_KEYWORD} " " DESKTOP_TEMP ${LINE_OUTPUT})
            string(STRIP ${DESKTOP_TEMP} DESKTOP_DIR)
            set(${DESKTOP_DIR_OUTPUT} ${DESKTOP_DIR} PARENT_SCOPE)
        endif()

        set(ICON_DIR_KEYWORD "Plugin icon dir: ")
        string(FIND ${LINE_OUTPUT} ${ICON_DIR_KEYWORD} CAN_FIND)
        if( NOT (${CAN_FIND} EQUAL -1) )
            string(REPLACE ${ICON_DIR_KEYWORD} " " ICON_TEMP ${LINE_OUTPUT})
            string(STRIP ${ICON_TEMP} ICON_DIR)
            set(${ICON_DIR_OUTPUT} ${ICON_DIR} PARENT_SCOPE)
        endif()
    endforeach()

endfunction(GET_CONTROL_CENTER_DIR)