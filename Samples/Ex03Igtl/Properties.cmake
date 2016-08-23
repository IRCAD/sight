
set( NAME Ex03Igtl )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE)
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    gui
    guiQt
    servicesReg
    fwlauncher
    appXml
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    media
    io
    ioVTK
    ioNetwork
    ioIGTL
    ioData
    uiIO
    uiIGTL
    )

bundleParam(appXml PARAM_LIST config PARAM_VALUES Ex03IgtlConfig)
