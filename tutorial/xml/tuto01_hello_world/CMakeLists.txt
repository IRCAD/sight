sight_add_target(tuto01_hello_world TYPE APP)

add_dependencies(tuto01_hello_world sightrun module_app module_ui_qt module_service module_ui)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES tuto01_hello_world)
