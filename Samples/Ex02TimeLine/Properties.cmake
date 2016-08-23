
set( NAME Ex02TimeLine )
set( VERSION 0.1 )
set( TYPE APP )
set( PRIORITY 3 )
set( DEPENDENCIES
    fwServices
    arData
    fwRuntime
    fwData
    fwCom
    fwThread
    fwTools
    fwCore

)
set( REQUIREMENTS
    appXml
    arDataReg
    servicesReg
    fwlauncher
    dataReg
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES Ex02TimeLineConfig)
