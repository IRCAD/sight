sight_add_target(app_test_app TYPE APP)

add_dependencies(app_test_app sightrun module_app module_service)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES app_test_app_cfg)
