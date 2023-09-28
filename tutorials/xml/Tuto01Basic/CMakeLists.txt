sight_add_target(Tuto01Basic TYPE APP)

add_dependencies(Tuto01Basic sightrun module_appXml module_ui_qt module_service module_ui)

# Main application's configuration to launch
module_param(module_appXml PARAM_LIST config PARAM_VALUES Tuto01Basic_AppCfg)

sight_generate_profile(Tuto01Basic)
