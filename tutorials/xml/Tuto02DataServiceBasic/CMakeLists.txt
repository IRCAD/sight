sight_add_target( Tuto02DataServiceBasic TYPE APP )

add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_qt
                 data
                 module_service
                 module_ui_base
                 module_io_vtk
)

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        TutoDataServiceBasic_AppCfg
) # Main application's configuration to launch
