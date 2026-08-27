sight_add_target(tuto01_hello_world TYPE APP)

add_dependencies(
    tuto01_hello_world # Our sample
    module_app # Launch an application written with XML extensions
    module_service # Register services extensions
    module_ui_qt # Load qt implementation of gui
    sightrun # Generic launcher for applications
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES tuto01_hello_world)
