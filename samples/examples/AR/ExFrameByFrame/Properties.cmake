
set( NAME ExFrameByFrame )
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
    appXml
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    visuVTKARAdaptor
    videoQt
    videoOpenCV
    videoTools
    preferences
    )

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExFrameByFrameConfig)
