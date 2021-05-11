sight_add_target( Tuto16SceneQt3D TYPE APP UNIQUE ON )


add_dependencies(Tuto16SceneQt3D 
                 sightrun
                 module_appXml
                 module_ui_qt
                 data
                 module_service
                 module_ui_base
                 module_io_vtk
                 module_io_atoms
                 module_viz_qt3d
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
        Tuto16SceneQt3D_AppCfg
) # Main application's configuration to launch

sight_generate_profile(Tuto16SceneQt3D)
