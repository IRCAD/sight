sight_add_target(tuto01_basic TYPE APP)

add_dependencies(tuto01_basic sightrun module_app module_ui_qt module_service module_ui)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES Tuto01Basic_AppCfg)

sight_generate_profile(tuto01_basic)
