sight_add_target( ExImageMix TYPE APP UNIQUE ON )

add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 data
                 module_service
                 module_ui_base
                 module_io_vtk
                 module_io_matrix
                 module_data
                 module_viz_scene3d
                 module_viz_scene3dQt
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
        ExImageMix_AppCfg
) # Main application's configuration to launch
