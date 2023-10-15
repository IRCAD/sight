sight_add_target(Tuto03DataService TYPE APP)

add_dependencies(
    Tuto03DataService
    sightrun
    module_app
    module_ui_qt
    data
    module_service
    module_ui
    module_io_vtk
    module_viz_sample
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES Tuto03DataService_AppCfg)

sight_generate_profile(Tuto03DataService)
