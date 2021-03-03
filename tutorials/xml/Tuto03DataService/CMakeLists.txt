sight_add_target( Tuto03DataService TYPE APP )

add_dependencies(${FWPROJECT_NAME} 
                 sightrun
                 module_appXml
                 module_ui_qt
                 data
                 module_service
                 module_ui_base
                 module_io_vtk
                 module_viz_sample
)

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto03DataService_AppCfg
) # Main application's configuration to launch

sight_generate_profile(${FWPROJECT_NAME})
