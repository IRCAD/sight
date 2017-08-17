
set( NAME TutoGui )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS 
    io
    appXml2
    gui
    guiQt
    dataReg
    servicesReg
    fwlauncher
)

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES tutoGuiConfig)
