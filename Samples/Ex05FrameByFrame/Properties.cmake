
set( NAME Ex05FrameByFrame )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE)
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    arDataReg
    gui
    guiQt
    servicesReg
    fwlauncher
    appXml2
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    visuVTKARAdaptor
    videoQt
    videoOpenCV
    videoTools
    arMedia
    preferences
    )

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES Ex05FrameByFrameConfig)
