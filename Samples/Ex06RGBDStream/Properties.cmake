
set( NAME Ex06RGBDStream )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES )

set( REQUIREMENTS
    appXml2
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

    media
    rdMedia
)

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES Ex06RGBDStream)
