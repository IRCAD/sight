sight_add_target( ExSolvePnP TYPE APP )


add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_base
                 module_viz_scene3d
                 module_ui_qt
                 module_viz_scene3dQt
                 data
                 module_service
                 module_ui_base
                 module_ui_flaticons
                 module_io_vtk
                 module_io_vision
                 module_io_video
                 module_sync
                 module_data
                 module_geometry_base
                 module_geometry_vision
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
        config
    PARAM_VALUES
        ExSolvePnP_AppCfg
) # Main application's configuration to launch

sight_add_shortcut(${FWPROJECT_NAME})
