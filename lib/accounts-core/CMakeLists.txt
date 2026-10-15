set(TARGET_NAME accounts-core)

file(GLOB_RECURSE ACCOUNTS_CORE_SRC "./*.cpp" "./*.h")

add_library(${TARGET_NAME} STATIC ${ACCOUNTS_CORE_SRC})

target_link_libraries(${TARGET_NAME}
        PUBLIC Qt5::Core
        Qt5::DBus
        PRIVATE dbus-wrapper
        ${KLOG_LIBRARIES})

target_include_directories(${TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/common
        ${KIRAN_CC_DAEMON_INCLUDE_DIRS}
        ${KLOG_INCLUDE_DIRS})
