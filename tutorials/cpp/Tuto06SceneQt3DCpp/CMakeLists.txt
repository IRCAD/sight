sight_add_target( Tuto06SceneQt3DCpp TYPE APP START ON )

add_dependencies(Tuto06SceneQt3DCpp 
                 sightrun
                 module_ui_base
                 module_ui_qt
                 module_viz_qt3d
                 data
                 module_service
                 module_ui_base
                 module_io_vtk
                 module_viz_qt3d
)

target_link_libraries(Tuto06SceneQt3DCpp PUBLIC 
                      data
                      service
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
        Tuto14MeshGenerator_AppCfg
) # Main application's configuration to launch

sight_generate_profile(Tuto06SceneQt3DCpp)
