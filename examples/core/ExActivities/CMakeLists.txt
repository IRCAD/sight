sight_add_target( ExActivities TYPE APP )


add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
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
                 2DVisualizationActivity
                 volumeRenderingActivity
                 config_ui_activity
                 module_viz_sample
)

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        sight_module_ui_qt/flatdark.rcc
        sight_module_ui_qt/flatdark.qss
) # Allow dark theme via module_ui_qt
moduleParam(
        module_appXml
    PARAM_LIST
        config parameters
    PARAM_VALUES
        ExActivities_Base_AppCfg ExActivities_AppCfgParam
) # Main application's configuration to launch

sight_add_shortcut(${FWPROJECT_NAME})
