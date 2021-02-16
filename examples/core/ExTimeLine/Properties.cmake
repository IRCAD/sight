set( NAME ExTimeLine )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES
    data
    service
    core
    exTimeLineData
)
set( REQUIREMENTS
    module_appXml
    module_service
    fwlauncher
    module_ui_console
    exTimeLineData
)

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExTimeLine_AppCfg
) # Main application's configuration to launch
