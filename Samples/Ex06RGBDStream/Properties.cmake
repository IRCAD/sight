
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
    uiTF

    visuVTK
    visuVTKQt
    visuVTKAdaptor
    visuVTKARAdaptor

    videoTools
    videoOpenni

    arMedia
    media
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES Ex06RGBDStream)
