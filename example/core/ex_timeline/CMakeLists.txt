sight_add_target(ex_timeline TYPE APP)

add_dependencies(ex_timeline module_app module_service sightrun module_ui_console)

target_include_directories(ex_timeline PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(ex_timeline PUBLIC data service core)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ex_timeline_AppCfg)
