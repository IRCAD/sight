
set( NAME PoC08Igtl )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE)
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    gui
    guiQt
    servicesReg
    visu
    launcher
    appXml
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    media
    mediaExt
    io
    ioVTK
    ioNetwork
    ioIGTL
    ioData
    uiIO
    uiNetwork
    uiIGTL
    )

bundleParam(appXml PARAM_LIST config PARAM_VALUES PoC08Igtl)
