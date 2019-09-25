
set( NAME TutoGui )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS 
    appXml
    gui
    guiQt
    dataReg
    servicesReg
    fwlauncher
    media
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES tutoGuiConfig)
