sight_add_target( Tuto06SceneQt3DCpp TYPE APP START ON )

add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_ui_base
                 module_ui_qt
                 module_viz_qt3d
                 data
                 module_service
                 module_ui_base
                 module_io_vtk
                 module_viz_qt3d
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      service
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
        Tuto14MeshGenerator_AppCfg
) # Main application's configuration to launch
