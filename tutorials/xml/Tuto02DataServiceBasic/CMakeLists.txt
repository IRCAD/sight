sight_add_target(Tuto02DataServiceBasic TYPE APP)

add_dependencies(
    Tuto02DataServiceBasic
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

sight_generate_profile(Tuto02DataServiceBasic)
