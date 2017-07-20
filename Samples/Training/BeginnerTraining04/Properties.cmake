
set( NAME BeginnerTraining04 )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    servicesReg
    dataReg
    gui
    guiQt
    io
    ioVTK
    uiIO
    fwlauncher
    appXml2
    beginnerTraining
    visuVTKQt
    visuVTKAdaptor
)

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES BeginnerTraining04Config)
