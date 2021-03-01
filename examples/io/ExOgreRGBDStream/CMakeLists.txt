sight_add_target( ExOgreRGBDStream TYPE APP UNIQUE ON )

add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_base
                 module_viz_scene3d
                 module_ui_qt
                 module_viz_scene3dQt
                 module_viz_scene2d
                 data
                 module_service
                 module_ui_base
                 module_ui_flaticons
                 module_io_video
                 module_sync
                 viz_scene2d
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
        ExOgreRGBDStream_AppCfg
) # Main application's configuration to launch

sight_generate_profile(${FWPROJECT_NAME})
