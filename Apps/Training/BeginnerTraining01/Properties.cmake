
set( NAME BeginnerTraining01 )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    servicesReg
    dataReg
    gui
    guiQt
    io
    fwlauncher
    appXml2
    beginnerTraining
)

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES BeginnerTraining01Config)
