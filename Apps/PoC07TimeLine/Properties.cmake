
set( NAME PoC07TimeLine )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES
    fwServices
    extData
    fwRuntime
    fwData
    fwCom
    fwThread
    fwTools
    fwCore

)
set( REQUIREMENTS 
    appXml 
    extDataReg 
    servicesReg 
    launcher
    dataReg
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES PoC07TimeLineConfig)