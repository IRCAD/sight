
set( NAME Ex06RGBDStream )
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
    videoQt
    videoTools

    arMedia
    media

    preferences
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES Ex06RGBDStream)
