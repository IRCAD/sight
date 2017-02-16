
set( NAME Ex03Igtl )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE)
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    gui
    guiQt
    servicesReg
    fwlauncher
    appXml2
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    media
    preferences
    io
    ioVTK
    ioNetwork
    ioIGTL
    ioData
    uiIO
    uiPreferences
    )

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES Ex03IgtlConfig)
