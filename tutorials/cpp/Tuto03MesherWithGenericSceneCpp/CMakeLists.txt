sight_add_target( Tuto03MesherWithGenericSceneCpp TYPE APP START ON )

add_dependencies(Tuto03MesherWithGenericSceneCpp 
                 module_data
                 data
                 module_ui_base
                 module_ui_qt
                 module_io_matrix
                 module_io_vtk
                 module_service
                 module_filter_mesh
                 module_viz_scene3d
                 module_viz_scene3dQt
                 module_ui_flaticons
                 sightrun
)

target_link_libraries(Tuto03MesherWithGenericSceneCpp PUBLIC data service)

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        sight::module::ui::qt/flatdark.rcc
        sight::module::ui::qt/flatdark.qss
) 

# Allow dark theme via module_ui_qt
moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto04MeshGeneratorCpp_AppCfg
) # Main application's configuration to launch

sight_generate_profile(Tuto03MesherWithGenericSceneCpp)
