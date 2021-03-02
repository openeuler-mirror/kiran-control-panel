cmake_minimum_required(VERSION 3.0)
project(plugin-demo VERSION 1.0)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

include(kiran_control_center_pulgin_dir)
include(qt)
include(app_plugin_var_declare)
include(app_plugin_var_define)
#添加子目录
add_subdirectory(desktop)
add_subdirectory(src)
