sight_add_target( Tuto10LaunchBasicConfig TYPE APP )



add_dependencies(Tuto10LaunchBasicConfig 
                 sightrun
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 data
                 module_service
                 module_ui_base
                 module_io_matrix
                 module_io_vtk
                 module_viz_sample
)

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        sight::module::ui::qt/flatdark.rcc
        sight::module::ui::qt/flatdark.qss
) # Allow dark theme via module_ui_qt
moduleParam(
    module_appXml
    PARAM_LIST
    config
    PARAM_VALUES
    Tuto10LaunchBasicConfig_AppCfg
) # Main application's configuration to launch

sight_generate_profile(Tuto10LaunchBasicConfig)
