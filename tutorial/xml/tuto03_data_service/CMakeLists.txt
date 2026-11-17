sight_add_target(tuto03_data_service TYPE APP)

add_dependencies(
    tuto03_data_service
    data
    module_app
    module_io_vtk
    module_service
    module_ui
    module_ui_qt
    module_viz_sample
    sightrun
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES tuto03_data_service)
