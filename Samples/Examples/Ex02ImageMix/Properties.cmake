
set( NAME Ex02ImageMix )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    ctrlSelection
    gui
    guiQt
    ioData
    ioVTK
    io
    servicesReg
    uiVisuQt
    uiImageQt
    uiIO
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    fwlauncher
    appXml2
)

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES Ex02ImageMix)
