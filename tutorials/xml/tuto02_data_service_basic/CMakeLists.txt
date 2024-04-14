sight_add_target(tuto02_data_service_basic TYPE APP)

add_dependencies(
    tuto02_data_service_basic
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
module_param(module_app PARAM_LIST config PARAM_VALUES TutoDataServiceBasic_AppCfg)

sight_generate_profile(tuto02_data_service_basic)
