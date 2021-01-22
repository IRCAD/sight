set( NAME ExTimeLine )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES
    arData
    fwData
    fwRuntime
    fwServices
    core
    exTimeLineData
)
set( REQUIREMENTS
    appXml
    servicesReg
    fwlauncher
    console
    exTimeLineData
)

moduleParam(
        appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExTimeLine_AppCfg
) # Main application's configuration to launch
