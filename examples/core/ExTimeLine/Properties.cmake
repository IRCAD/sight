set( NAME ExTimeLine )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES
    data
    services
    core
    exTimeLineData
)
set( REQUIREMENTS
    module_appXml
    module_services
    fwlauncher
    console
    exTimeLineData
)

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExTimeLine_AppCfg
) # Main application's configuration to launch
