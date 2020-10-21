set( NAME ExTimeLine )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES
    arData
    fwData
    fwServices
)
set( REQUIREMENTS
    appXml
    fwData
    arData
    servicesReg
    fwlauncher
    console
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExTimeLineConfig)
