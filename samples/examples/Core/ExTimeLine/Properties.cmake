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
    dataReg
    arDataReg
    servicesReg
    fwlauncher
    console
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExTimeLineConfig)
