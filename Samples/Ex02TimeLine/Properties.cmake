set( NAME Ex02TimeLine )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES
    arData
    fwData
    fwServices
)
set( REQUIREMENTS
    appXml2
    dataReg
    arDataReg
    servicesReg
    fwlauncher
    console
)

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES Ex02TimeLineConfig)
