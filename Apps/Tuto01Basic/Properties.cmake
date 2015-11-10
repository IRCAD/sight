
set( NAME Tuto01Basic )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS 
    io
    appXml
    gui
    guiQt
    dataReg
    servicesReg
    launcher
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES tutoBasicConfig)
