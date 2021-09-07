sight_add_target( ExActivities TYPE APP )


add_dependencies(ExActivities 
                 sightrun
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 module_activity
                 data
                 module_service
                 module_ui_base
                 module_ui_flaticons
                 module_io_atoms
                 module_io_vtk
                 module_io_activity
                 activity_viz_negato
                 config_ui_activity
                 module_viz_sample
                 module_viz_scene3d
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
        config parameters
    PARAM_VALUES
        ExActivities_Base_AppCfg ExActivities_AppCfgParam
) # Main application's configuration to launch

sight_generate_profile(ExActivities)
