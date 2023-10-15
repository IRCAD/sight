sight_add_target(ExTimeLine TYPE APP)

add_dependencies(ExTimeLine module_app module_service sightrun module_ui_console)

target_include_directories(ExTimeLine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(ExTimeLine PUBLIC data service core)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ExTimeLine_AppCfg)

sight_generate_profile(ExTimeLine)
