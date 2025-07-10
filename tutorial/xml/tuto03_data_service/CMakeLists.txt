sight_add_target(tuto03_data_service TYPE APP)

add_dependencies(
    tuto03_data_service
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
module_param(module_app PARAM_LIST config PARAM_VALUES tuto03_data_service)
