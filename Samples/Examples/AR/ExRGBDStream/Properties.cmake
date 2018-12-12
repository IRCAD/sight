
set( NAME ExRGBDStream )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES )

set( REQUIREMENTS
    appXml
    fwlauncher

    arDataReg
    dataReg
    servicesReg

    gui
    guiQt

    scene2D

    visuVTK
    visuVTKQt
    visuVTKAdaptor
    visuVTKARAdaptor

    videoOpenni
    videoOpenCV
    videoOrbbec
    videoQt
    videoTools

    arMedia
    media

    preferences
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExRGBDStream)
