sight_add_target( Tuto07GenericScene TYPE APP UNIQUE ON )


add_dependencies(Tuto07GenericScene 
                 sightrun
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 module_viz_scene3d
                 module_viz_scene3dQt
                 data
                 module_service
                 module_ui_flaticons
                 module_io_matrix
                 module_io_vtk
                 module_io_atoms
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
        Tuto07GenericScene_AppCfg
) # Main application's configuration to launch

sight_generate_profile(Tuto07GenericScene)
