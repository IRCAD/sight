
set( NAME ExRGBDStream )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES )

set( REQUIREMENTS
    appXml
    fwlauncher

    arData
    fwData
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
    videoRealSense
    videoQt
    videoTools
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExRGBDStream)
